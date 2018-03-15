/*
   robot.cpp

    Created on: Feb 19, 2018
        Author: koldar
*/

#include "robot.hpp"
#include <Wire.h>

extern Zumo32U4LCD lcd;
extern L3G gyro;
extern LSM303 accel;
extern Zumo32U4Encoders encoders;
extern Zumo32U4LineSensors lineSensors;

uint32_t turnAngle = 0;
int16_t turnRate;
int16_t gyroOffset;
uint16_t gyroLastUpdate = 0;



namespace robotieee {

  /**
   * the light intensity dected by one of the line sensor
   */
  enum line_color {
    LC_WHITE,
    LC_LIGHTGRAY,
    LC_DARKGRAY,
    LC_BLACK,
  };

  /**
   * values goees from 0 to X, where x depends on how much is the timeout of the line sensors.
   * Default maximum value is 2000.
   */
  struct line_readings {
    enum line_color leftMost;
    enum line_color left;
    enum line_color center;
    enum line_color right;
    enum line_color rightMost;
  };

  enum line_sensors {
    LEFTMOST = 0,
    LEFT = 1,
    CENTER = 2,
    RIGHT = 3,
    RIGHTMOST = 4
  }

  static struct line_readings readLineSensors();

  robot::robot(const point& start_position) : moveable{start_position} {
  
  }
  
  robot::~robot() {
  
  }

  void robot::hardwareInit() {
    Wire.begin();
    lcd.init();
    accel.init();
    accel.setTimeout(500);
    accel.enableDefault();
    gyro.init();
    gyro.setTimeout(500);
    gyro.enableDefault();
    //enable left-most sensor, center sensor and right-most sensor
    //lineSensors.initThreeSensors(); 
    lineSensors.initFiveSensors(); 
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

  void robot::followLine(int speed, int speedCompensation) {

    int sxMotor = speed;
    int dxMotor = speed;

    while (true) {
      Zumo32U4Motors::setSpeeds(sxMotor, dxMotor);
      
      struct line_readings lineReadings = readLineSensors();

      /* there the following scenarios:
       * WWW: not possible because it means the robot outside any line
       * WWB: not possible because it is already handle by WBB
       * WBW: when the robot is going straight
       * WBB: when the robot is going out of trail on the left
       * BWW: not possible because it is already handled by BBW
       * BWB: not possible because we should be on the line
       * BBW: when the robot is going out of trail on the right
       * BBB: the line is by assumption thin. this case happens only when we've reached a black cross
      */

      if (lineReadings.left == LC_BLACK && lineReadings.center == LC_BLACK && lineReadings.right == LC_BLACK) {
        break;
      }

      if (lineReadings.left == LC_WHITE && lineReadings.center == LC_BLACK && lineReadings.right == LC_WHITE) {
        sxMotor = speed;
        dxMotor = speed;
        continue; 
      }

       //technically check the center is useless
      if (lineReadings.left == LC_BLACK && lineReadings.center == LC_BLACK && lineReadings.right == LC_WHITE) {
        sxMotor -= speedCompensation;
        dxMotor += speedCompensation;
      }

      if (leftReadings.left == LC_WHITE && lineReadings.center == LC_BLACK && lineReadings.right == LC_BLACK) {
        sxMotor += speedCompensation;
        dxMotor -= speedCompensation;
      }
      
    }

    //stop motors
    Zumo32U4Motors::setSpeeds(0, 0);
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

static enum line_color convertValueToLineColor(int value) {
  if (value < 500) {
    return LC_WHITE;
  }
  if (value < 1000) {
    return LC_LIGHTGRAY;
  }
  if (value < 1500) {
    return LC_DARKGRAY;
  }
  return LC_BLACK;
}
/**
 * Read the values of left, center and right line tracking
 */
static struct line_readings readLineSensors() {
  int tmp[3];
  struct line_readings retVal;
  
  lineSensors.read(tmp);

  retVal.leftMost = convertValueToLineColor(tmp[0]);
  retVal.left = convertValueToLineColor(tmp[1]);
  retVal.center = convertValueToLineColor(tmp[2]);
  retVal.right = convertValueToLineColor(tmp[3]);
  retVal.rightMost = convertValueToLineColor(tmp[4]);

  return retVal;
}

