/*
 * robot.cpp
 *
 *  Created on: Feb 19, 2018
 *      Author: koldar
 */

#include "robot.hpp"

extern Zumo32U4LCD lcd;
extern L3G gyro;
extern LSM303 accel;

namespace robotieee {

robot::robot(const point& start_position) : moveable{start_position} {
  
}

robot::~robot() {

}

void robot::rotate(int16_t degrees) {
  uint32_t timeNeeded;
  int speed = MOTORS_POWER;

  //Choose correct averate angular rate and eventually flip motor speeds if needed
  if (degrees > 0) {
     timeNeeded = abs(degrees) / _averageAngularRateCcw * 1000;
  }
  else {
    timeNeeded = abs(degrees) / _averageAngularRateCw * 1000;
    speed = - speed;

  }
  
  Zumo32U4Motors::setSpeeds(-speed, speed);
  delay(timeNeeded);
  Zumo32U4Motors::setSpeeds(0, 0);
  delay(MOVEMENT_DELAY);
  
}

void robot::moveStraight(int16_t centimeters){
  uint32_t timeNeeded;
  int speed = MOTORS_POWER;

  if (centimeters > 0) {
    timeNeeded = sqrt ( (abs(centimeters)/1000) / _averageSpeedFw ); 
  }

  else {
    timeNeeded = sqrt ( (abs(centimeters)/1000) / _averageSpeedBw );
  }

  Zumo32U4Motors::setSpeeds(speed, speed);
  delay(timeNeeded);
  Zumo32U4Motors::setSpeeds(0, 0);
  delay(MOVEMENT_DELAY);
}

void robot::calibrateAccelerometer(){
  float maxAccelValue;
  int n_measurements
  int power = MOTORS_POWER;

  for (int i = 0; i < 2; i++) {
    maxAccelValue = 0.0f;
    n_measurements = 0;

    Zumo32U4Motors::setSpeeds(power, power);
    while (n_measurements < ACCEL_CALIBRATION_MEASUREMENTS){
      accel.readAcc();
      if (abs(accel.a.x) > maxAccelValue) maxAccelValue = abs(accel.a.x);
      n_measurements++;
    }
    Zumo32U4Motors::setSpeeds(0,0);
    delay(MOVEMENT_DELAY);

    //Forward calibration on first iteration, backwards on second
    if (i == 0) {
      _averageAccelFw = maxAccelValue * ACCEL_SENSITIVITY;
    }
    else {
      _averageAccelBw = maxAccelValue * ACCEL_SENSITIVITY;
    }
    power = -power
  }
}

void robot::calibrateGyroscope() {
  float averageReadValue;
  int measurements;
  int power = MOTORS_POWER;

  //On first iteration we calibrate counter-clockwise rotation, on second one the other
  for (int i = 0; i < 2; i++) {
    averageReadValue = 0.0f;
    measurements = 0;
    
    Zumo32U4Motors::setSpeeds(-power, power);
    while (measurements < GYRO_CALIBRATION_MEASUREMENTS) {
      gyro.read();
      averageReadValue += abs(gyro.g.z) / (float) GYRO_CALIBRATION_MEASUREMENTS;
      measurements++;
    }
    Zumo32U4Motors::setSpeeds(0, 0);
    delay(MOVEMENT_DELAY);

    //Counter-clockwise on first iteration, clockwise on second
    if (i == 0) {
      _averageAngularRateCcw = averageReadValue * GYRO_SENSITIVITY;
    }
    else {
      _averageAngularRateCw = averageReadValue * GYRO_SENSITIVITY;
    }

    //Invert motor speeds
    power = -power;
    
  }
}

}


