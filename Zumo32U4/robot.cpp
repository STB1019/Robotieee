/*
   robot.cpp

    Created on: Feb 19, 2018
        Author: koldar
*/
#include "robot.hpp"
#include "TurnSensor.h"
#include <Wire.h>

#define DEFAULT_ORIENTATION               object_movement::DOWN
#define DEFAULT_SPEED                     150
#define DEFAULT_CENTERING_DELAY           167
#define DEFAULT_PATH_SEEK_COMPENSATION    5
#define DEFAULT_SPEED_COMPENSATION        5
#define DEFAULT_BACKWARDS_CENTERING_DELAY 0
#define DEFAULT_BLOCK_CENTERING_DELAY     220

extern L3G gyro;
extern LSM303 accel;
extern Zumo32U4Buzzer buzzer;
extern Zumo32U4LineSensors lineSensors;
extern Zumo32U4ProximitySensors proxSensors;

#ifdef DEBUG_LCD
extern Zumo32U4LCD lcd;
#endif

namespace robotieee {

  static struct line_readings readLineSensors();

  robot::robot(const point start_position) : moveable{start_position} {
    _hardwareInitialized   = false;
    _scanning              = true;
    _speed                 = DEFAULT_SPEED;
    _centeringDelay        = DEFAULT_CENTERING_DELAY;
    _pathSeekCompensation  = DEFAULT_PATH_SEEK_COMPENSATION;
    _speedCompensation     = DEFAULT_SPEED_COMPENSATION;
    _blockCenteringDelay   = DEFAULT_BLOCK_CENTERING_DELAY;
    _orientation           = DEFAULT_ORIENTATION;
  }
  
  robot::~robot() {
  
  }

  void robot::hardwareInit() {

    if (_hardwareInitialized) {
      return;
    }

#ifdef DEBUG_LCD
    lcd.init();
#endif
    
    // At the moment, the gyroscope is initialized by TurnSensor.cpp code
    //gyro.init();
    //gyro.setTimeout(500);
    //gyro.enableDefault();
   
    lineSensors.initThreeSensors(); 
    proxSensors.initThreeSensors(); 
    
    // Gyroscope offset calibration
    turnSensorSetup();

    // Manual line sensor calibration
    calibrateLineSensors();

    _hardwareInitialized = true;
  }
  
  
  bool robot::rotate(int16_t degrees, bool stopIfCenterBlack = false) {

    int sign = (degrees > 0) ? 1 : -1;
    bool retVal;

    turnSensorReset();
    Zumo32U4Motors::setSpeeds(-sign * _speed, sign * _speed);
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

  bool robot::rotateAndCheck(int16_t degrees) {
    rotate(degrees, false);
    return checkForBlock();
  }

  void robot::faceDirection(object_movement targetDirection) {

    // First of all we rotate the reference system so that we are sure that the initial orientation of the robot becomes UP
    object_movement rotatedTargetOrientation = (targetDirection - _orientation + 4) % 4;

    // After this rotation it is trivial to determine how to turn the robot by checking rotated target direction
    switch (rotatedTargetOrientation) {

      case 1: turnRight();
              break;
              
      case 2: turnBack();
              break;
              
      case 3: turnLeft();
              break;
              
    }
  }
  
  bool robot::followLine(bool searchBlock = false) {

    int sxSpeed = _speed;
    int dxSpeed = _speed;

    bool blockFound = false;
    
    while (true) {
      
      Zumo32U4Motors::setSpeeds(sxSpeed, dxSpeed);
      struct line_readings lineReadings = readLineSensors();

      //proximity check
      if (searchBlock == true && blockFound == false) {
        blockFound = checkForBlock();
      }
      
      /* The possible scenarios are:
       * - WWW: the robot has just abandoned the path
       * - WWB: we lost the path but we know that we are going left too much
       * - WBW: we are correctly following the path
       * - WBB: we reached the intersection but we are a bit off from the track
       * - BWW: we lost the path but we know that we are going right too much
       * - BWB: we are looking for the path we lost
       * - BBW: we reached the intersection but we are a bit off from the track
       * - BBB: we reached the intersection with no relevant error
       */

      if (lineReadings.left == LC_BLACK && lineReadings.center == LC_BLACK && lineReadings.right == LC_BLACK) {
        // The delay is used to make sure that the robot reaches the center of the intersection
        // and does not stop as soon as it sees the black horizontal line
        delay(_centeringDelay);
        break;
      }

      if (lineReadings.left == LC_WHITE && lineReadings.center == LC_BLACK && lineReadings.right == LC_WHITE) {
        sxSpeed = _speed;
        dxSpeed = _speed;
        continue; 
      }

      if (lineReadings.center == LC_WHITE) {
        fixPath();
        continue;
      }

      if (lineReadings.left == LC_BLACK && lineReadings.center == LC_BLACK && lineReadings.right == LC_WHITE) {
        dxSpeed += _speedCompensation;
      }

      if (lineReadings.left == LC_WHITE && lineReadings.center == LC_BLACK && lineReadings.right == LC_BLACK) {
        sxSpeed += _speedCompensation;
      }
      
    }

    //stop motors
    Zumo32U4Motors::setSpeeds(0, 0);

    return blockFound;
  }

  void robot::fixPath() {

    int i = 1;

    while (true) {
      bool foundBlack = rotate(i * _pathSeekCompensation, true);
      if (foundBlack) {
        break;
      }

      i = -(i * 2);
    }
    
  }

  bool robot::followLineAndCheck() {
    return followLine(true);
  }

  bool robot::checkForBlock() {
    proxSensors.read();
    byte frontLeft = proxSensors.countsFrontWithLeftLeds();
    byte frontRight = proxSensors.countsFrontWithRightLeds();

#   if defined DEBUG_LCD && defined DEBUG_PROXIMITY
      //lcd.clear();
      lcd.gotoXY(0,1);
      lcd.print(frontLeft);
      lcd.print(frontRight);
#   endif
    
    if (frontLeft + frontRight >= 11) { //case sensors: 6 6; 6 5; 5 6
      return true;
    }

    return false;
  }

  void robot::calibrateLineSensors() {

#   ifdef DEBUG_LCD
    lcd.clear();
    lcd.print(F("White"));
    lcd.gotoXY(0,1);
    lcd.print(F("A for ok"));
#   endif

    ledRed(true);
    buttonA.waitForButton();
    lineSensors.calibrate();
    ledRed(false);

    delay(750);

#   ifdef DEBUG_LCD
    lcd.clear();
    lcd.print(F("Black"));
    lcd.gotoXY(0,1);
    lcd.print(F("A for ok"));
#   endif

    ledRed(true);
    buttonA.waitForButton();
    lineSensors.calibrate();
    ledRed(false);

    delay(750);

#   ifdef DEBUG_LCD
    lcd.clear();
#   endif
    
  }

  bool robot::turnRightAndCheck() {
    rotateAndCheck(-90);

    _orientation = (_orientation + 1) % 4;
  }

  bool robot::turnLeftAndCheck() {
    rotateAndCheck(90);

    _orientation = (_orientation + 3) % 4;
  }

  bool robot::turnBackAndCheck() {
    // A value of 179 degrees is used due to the way TurnSensor.cpp encodes degrees: a value of 180 would overflow and therefore not work
    // This isn't that bad after all: rotating 179 degrees + error should lead to a almost perfect 180 degrees turn anyway
    rotateAndCheck(179);

    _orientation = (_orientation + 2) % 4;
  }

  void robot::turnRight() {
    
    rotate(-90);
    _orientation = (_orientation + 1) % 4;
    
  }

  void robot::turnLeft() {
    
    rotate(90);
    _orientation = (_orientation + 3) % 4;
    
  }

  bool robot::isScanning() {
    return _scanning;
  }
  
  void robot::setScanMode() {
    _scanning = true;
    ledGreen(false);
    ledYellow(true);
    
  }

  void robot::setExecuteMode() {
    _scanning = false;
    ledGreen(true);
    ledYellow(false);
  }

  void robot::turnBack() {
    
    // A value of 179 degrees is used due to the way TurnSensor.cpp encodes degrees: a value of 180 would overflow and therefore not work
    // This isn't that bad after all: rotating 179 degrees + error should lead to a almost perfect 180 degrees turn anyway
    rotate(179);
    _orientation = (_orientation + 2) % 4;
    
  }

  bool robot::goAhead(unsigned int cells, bool lookingForBlocks = false) {
    bool blockFound = false;

    // Before starting we check if the block is immediately in front of us to avoid colliding
    if (lookingForBlocks) {
      
      blockFound = checkForBlock();
      
    }
    
    for (int i = 0; i < cells && !blockFound; i++) {
      
      blockFound = followLine(lookingForBlocks);
      move(_orientation, 1);
      
    }

    if (blockFound) {
      
      Zumo32U4Buzzer::playNote(NOTE_A(4), 300, 8);
      
    }
    
    return blockFound;
  }

  void robot::setSpeed(int16_t speed) {
    _speed = speed;
  }

  void robot::setSpeedCompensation(int16_t speedCompensation) {
    _speedCompensation = speedCompensation;
  }

  static enum line_color convertValueToLineColor(int value, bool invertWhite) {

    if (invertWhite) {
      if (value < 500) {
        return LC_BLACK;
      }
      /**
      if (value < 500) {
        return LC_LIGHTGRAY;
      }
      if (value < 750) {
        return LC_DARKGRAY;
      }
      */
      return LC_WHITE;
    }
    else {
      if (value < 500) {
        return LC_WHITE;
      }
      /**
      if (value < 500) {
        return LC_DARKGRAY;
      }
      if (value < 750) {
        return LC_LIGHTGRAY;
      }
      */
      return LC_BLACK;
    }
  }
  /**
   * Read the values of left, center and right line tracking
   */
  static struct line_readings readLineSensors() {
    int tmp[3];
    struct line_readings retVal;
    
    lineSensors.readCalibrated(tmp);
  
    retVal.left = convertValueToLineColor(tmp[LEFT_SENSOR], false);
    retVal.center = convertValueToLineColor(tmp[CENTER_SENSOR], false);
    retVal.right = convertValueToLineColor(tmp[RIGHT_SENSOR], false);

#   if defined DEBUG_LCD && defined DEBUG_LINE
    //lcd.clear();
    for (int i = 0; i < 3; i++) {
      lcd.gotoXY(i, 0);
      switch (convertValueToLineColor(tmp[i], false)){
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
  
  void robot::timeMove(uint16_t delayMillis) {
    Zumo32U4Motors::setSpeeds(_speed, _speed);
    delay(delayMillis);
    Zumo32U4Motors::setSpeeds(0,0);
  }

  void robot::invertSpeed(){
    setSpeed(-(_speed));
    setSpeedCompensation(-(_speedCompensation)); // Not useful at the moment
  }

  void robot::setCenteringDelay(uint16_t centeringDelay){
    _centeringDelay = centeringDelay;
  }
  
  void robot::pushBlock(unsigned int cells){

    // Push the block on the desired location
    goAhead(cells);
    timeMove(_blockCenteringDelay);

    // Invert the speed used for consequent movements, in order to move backwards
    invertSpeed();

    //It goes back the same time as the robot push the block for centering
    timeMove(_blockCenteringDelay);

    // Invert speed again to reset forward movement
    invertSpeed();
  }
}
