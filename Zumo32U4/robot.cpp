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
extern Zumo32U4Encoders encoders;

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

void robot::calibrateMovement(double distance) {
  int acceleration;
  unsigned long t;
  unsigned long newtime;
  float dtime;
  float timeNeeded;
  float velocity = 0;
  float n_velocity;
  
  delay(2000);
  Zumo32U4Motors::setSpeeds(150,150);
  t = micros();
  do {
    accel.readAcc();
    newtime = micros();
    dtime = (float) (newtime - t) / 1000000;
    acceleration = abs(accel.a.x) * G_ACCELERATION * ACCEL_SENS;
    if (accel.a.x > 500) {
      n_velocity = velocity + acceleration * dtime;
      distance = distance - (velocity + n_velocity) / 2 * dtime;
      velocity = n_velocity;
      t = newtime;
    }
    Serial.println(distance);
  } while (distance > 0);
  Zumo32U4Motors::setSpeeds(0,0);
}

void robot::moveStraight(float centimeters){

  float time_needed;
  time_needed = (centimeters / SPEED_100)*1000;
  if (centimeters > 0) {
    Zumo32U4Motors::setSpeeds(100,100);
  }
  else{
    Zumo32U4Motors::setSpeeds(-100,-100);
  }
  delay(time_needed);
  Zumo32U4Motors::setSpeeds(0,0);

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


