#include "Motors.cpp"
#include "API.cpp"
//#include "Scoreboard.cpp"

#define DEBUGGING     true
#define DEBUG_LED     13
#define SCORE_L_PIN   9
#define SCORE_R_PIN   8
#define BMONITOR_PIN  0
#define PHOTO_THRESHOLD 500

// Calibration value for photoresistor
int CAL_PHOTO_THRESHOLD;
boolean newData;

PololuMotors throwMotors;
API apiHandle;
//Scoreboard scoreboard(2, 25);
apiCall waitingAPICmd;
static const apiCall noAPICmd;

void setup()
{
  Serial.begin(115200);
  Serial.println("ROB421 Sensor Integration Demo");

  throwMotors.begin();

  pinMode(DEBUG_LED,    OUTPUT);  // debugging led
  pinMode(SCORE_L_PIN,  OUTPUT);  // scoreboard left digit
  pinMode(SCORE_R_PIN,  OUTPUT);  // scoreboard right digit
  pinMode(BMONITOR_PIN, INPUT);   // photoresistor

  delay(1000);
  long photo_sum = 0;
  for(int i=0; i<500; i++){
    photo_sum += analogRead(BMONITOR_PIN);
  }
  CAL_PHOTO_THRESHOLD = photo_sum / 500;
  CAL_PHOTO_THRESHOLD -= CAL_PHOTO_THRESHOLD/4;
  
  if(DEBUGGING){
    Serial.print("CAL_PHOTO_THRESHOLD = ");
    Serial.println(CAL_PHOTO_THRESHOLD); 
  }
}

/*
 * Main program loop
 */
void loop()
{
  // Check for new API commands

  if(apiHandle.listen()) {
    waitingAPICmd = apiHandle.getAPICall();
    if(waitingAPICmd.isReady){
      waitingAPICmd.isReady = false;
//      apiHandle.showAPICall();

      switch(waitingAPICmd.fnNum){
        case -1:
          throwMotors.decSpeed();
          break;
        case 1:
          throwMotors.setMotorValues(waitingAPICmd.arg1, waitingAPICmd.arg2);
          break;
        case 2:
          throwMotors.setRatio(waitingAPICmd.arg1, waitingAPICmd.arg2);
          break;
        case 3:
          throwMotors.setSpeed(waitingAPICmd.arg1);
          break;
        case 9:
          throwMotors.firingSequence();
          break;
        default:
          if( 10 <= waitingAPICmd.fnNum && waitingAPICmd.fnNum <= 100 ){
            // Set motor speed to a percentage (this is getting messy)
            throwMotors.setSpeed(4 * waitingAPICmd.fnNum);
          } else {
            Serial.print(waitingAPICmd.fnNum);
            Serial.println(" is not a valid command.");
          }
      }
      throwMotors.display();
    }
  }

  // Check if the ball is ready to fire
  if(analogRead(BMONITOR_PIN) < CAL_PHOTO_THRESHOLD){
    Serial.print("Ball detected: ");
    Serial.println( analogRead(BMONITOR_PIN) );
    throwMotors.firingSequence();
  }
}
