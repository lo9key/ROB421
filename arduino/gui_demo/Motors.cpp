// Motors.cpp
#include "Arduino.h"
#include "DualG2HighPowerMotorShield.h"
#include <ServoTimer4.h>

#define SERVO_DELAY 2000
#define MAX_SPEED 400
#define MIN_SPEED 0
#define THROW_SERVO_PIN 11

class PololuMotors
{
  DualG2HighPowerMotorShield24v14 md;
  ServoTimer4 throwServo;

  public:
  int spinVal;
  int maxSpeed;

  /*
   * Constructor
   */
  void begin( void )
  {
      maxSpeed = 0;
      spinVal = 50;
      md.init();
      md.calibrateCurrentOffsets();
      md.disableDrivers();
      throwServo.attach(THROW_SERVO_PIN);
      throwServo.write(2500);
  }

  /*
   * Update the total motor speed, then set both motors using their current ratios
   */
  void setSpeed( int newSpeed )
  {
    int LSpeed = min(50, spinVal) * newSpeed / 12.5;
    int RSpeed = (100 - max(50, spinVal)) * newSpeed / 12.5;

//    Serial.print("setSpeed using: ");
//    Serial.print(LSpeed);
//    Serial.print(", ");
//    Serial.println(RSpeed);
    
    md.setM1Speed( LSpeed );
    md.setM2Speed( RSpeed );
  }

  /*
   * Firing sequence to throw a ball
   */
  void firingSequence()
  {
    Serial.print("Throwing at ");
    Serial.print(maxSpeed);
    Serial.print(" (");
    Serial.print(spinVal);
    Serial.print(") :");

    md.enableDrivers();
    delay(1);  // The drivers require a maximum of 1ms to elapse when brought out of sleep mode.

    // range is -400 (reverse) to 400 (forward)
    for (int i = 0; i <= maxSpeed; i++) {
      if(i % 10 == 0) { Serial.print('+'); }
      setSpeed(i);
      delay(2);
    }
    // Spin up the throwing motors
    delay(SERVO_DELAY);

    // Move the firing servo arm
    throwServo.write(0);
    delay(1500);
    throwServo.write(2500);

    // Ball is gone, might as well wind down
    for (int i = maxSpeed; i >= 0; i--) {
      if(i % 10 == 0) { Serial.print('-'); }
      setSpeed(i);
      delay(2);
    }

    // Leave motors disabled when not intentionally using them
    md.disableDrivers();

    Serial.println(" OK.");
  }

};
