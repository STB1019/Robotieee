/*
   robot.cpp

    Created on: Feb 19, 2018
        Author: koldar
*/

#include "robot.hpp"
#include "TurnSensor.h"
#include <Wire.h>

extern Zumo32U4LCD lcd;
extern L3G gyro;
extern LSM303 accel;
extern Zumo32U4Encoders encoders;
extern Zumo32U4LineSensors lineSensors;

namespace robotieee {

  static struct line_readings readLineSensors();

  robot::robot(const point& start_position) : moveable{start_position} {
  
  }
  
  robot::~robot() {
  
  }

  void robot::hardwareInit() {
    //Wire.begin();
    lcd.init();
    accel.init();
    accel.setTimeout(500);
    accel.enableDefault();
    //gyro.init();
    //gyro.setTimeout(500);
    //gyro.enableDefault();
   
    // Disable LED pulses readings
    lineSensors.initThreeSensors(); 
    
    // Gyroscope offset calibration
    turnSensorSetup();

    // Manual line sensor calibration
    calibrateLineSensors();
  }
  
  
  bool robot::rotate(int16_t degrees, unsigned int speed, bool stopIfCenterBlack = false) {

    int sign = (degrees > 0) ? 1 : -1;
    bool retVal;

    turnSensorReset();
    Zumo32U4Motors::setSpeeds(-sign * speed, sign * speed);
    while (true) {
      turnSensorUpdate();

      // If the amount of degrees rotated exceeds the desired rotation, stop
      if (abs((int32_t) turnAngle) >= abs(degrees * turnAngle1)) {
        retVal = false;
        break;
      }

      // Check for the center line sensor if requested and stop if a black line is found
      if (stopIfCenterBlack) {
        struct line_readings readings = readLineSensors();
        if (readings.center == LC_BLACK) {
          retVal = true;
          break;
        }
      }
    }
    Zumo32U4Motors::setSpeeds(0, 0);
    return retVal;
  
  }

  void robot::followLine(unsigned int speed, unsigned int delta) {

    int sxMotor = speed;
    int dxMotor = speed;

    while (true) {
      
      Zumo32U4Motors::setSpeeds(sxMotor, dxMotor);
      struct line_readings lineReadings = readLineSensors();

      /* The possible scenarios are:
       * x- WWW: the robot has just abandoned the path
       * x- WWB: we lost the path but we know that we are going left too much
       * x - WBW: we are correctly following the path
       * - WBB: we reached the intersection but we are a bit off from the track
       * x- BWW: we lost the path but we know that we are going right too much
       * x- BWB: we are looking for the path we lost
       * - BBW: we reached the intersection but we are a bit off from the track
       * x - BBB: we reached the intersection with no relevant error
       */

      if (lineReadings.left == LC_BLACK && lineReadings.center == LC_BLACK && lineReadings.right == LC_BLACK) {
        break;
      }

      if (lineReadings.left == LC_WHITE && lineReadings.center == LC_BLACK && lineReadings.right == LC_WHITE) {
        sxMotor = speed;
        dxMotor = speed;
        continue; 
      }

      if (lineReadings.center == LC_WHITE) {
        fixPath(speed, delta);
        continue;
      }

      if (lineReadings.left == LC_BLACK && lineReadings.center == LC_BLACK && lineReadings.right == LC_WHITE) {
        dxMotor += delta;
      }

      if (lineReadings.left == LC_WHITE && lineReadings.center == LC_BLACK && lineReadings.right == LC_BLACK) {
        sxMotor += delta;
      }
      
    }

    //stop motors
    Zumo32U4Motors::setSpeeds(0, 0);
  }

  void robot::fixPath(unsigned int speed, unsigned int delta) {

    int i = 1;

    while (true) {
      bool foundBlack = rotate(i * delta, speed, true);
      if (foundBlack) {
        break;
      }

      i = -(i * 2);
    }
    
  }

  void robot::calibrateLineSensors() {

    lcd.clear();
    lcd.print(F("White"));
    lcd.gotoXY(0,1);
    lcd.print(F("A for ok"));

    buttonA.waitForButton();
    lineSensors.calibrate();

    lcd.clear();
    lcd.print(F("Black"));
    lcd.gotoXY(0,1);
    lcd.print(F("A for ok"));

    buttonA.waitForButton();
    lineSensors.calibrate();

    lcd.clear();
    
  }

  static enum line_color convertValueToLineColor(int value) {
    if (value < 500) {
      return LC_WHITE;
    }
    /**
    if (value < 500) {
      return LC_LIGHTGRAY;
    }
    if (value < 750) {
      return LC_DARKGRAY;
    }
    */
    return LC_BLACK;
  }
  /**
   * Read the values of left, center and right line tracking
   */
  static struct line_readings readLineSensors() {
    int tmp[3];
    struct line_readings retVal;
    
    lineSensors.readCalibrated(tmp);
  
    retVal.left = convertValueToLineColor(tmp[0]);
    retVal.center = convertValueToLineColor(tmp[1]);
    retVal.right = convertValueToLineColor(tmp[2]);

#   ifdef DEBUG
    lcd.clear();
    for (int i = 0; i < 3; i++) {
      lcd.gotoXY(i, 0);
      switch (convertValueToLineColor(tmp[i])){
        case LC_WHITE: lcd.print(F("W"));
                       break;
        case LC_LIGHTGRAY: lcd.print(F("g"));
                       break;
        case LC_DARKGRAY: lcd.print(F("G"));
                       break;
        case LC_BLACK: lcd.print(F("B"));
                       break;
        
      }
    }
#   endif

  
    return retVal;
  }
  
}

