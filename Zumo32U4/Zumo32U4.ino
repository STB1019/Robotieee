#define AVR_BUILD
#define DEFAULT_ID 0 // Used as we do not care about blocks ids in this version of the code

#include <Zumo32U4.h>
#include "robot.hpp"
#include "block.hpp"
#include "actionComunicator.hpp"
#include "compositeAction.hpp"

using namespace robo_utils;
using namespace robotieee;

L3G gyro;
LSM303 accel;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensors;
actionComunicator bluetooth;

#ifdef DEBUG_LCD
Zumo32U4LCD lcd;
#endif

// CARE: The robot needs to be placed in the upper-left corner of the grid and facing towards the bottom of the grid
// !!!!!! CARE !!!!!!
// THE POINT INITIALIZATION SYNTAX IS (Y,X)
robot zumo_robot{(point) {0, 0}};

/*
 * This function returns true if the block was found during the movement. If this happens a LOCATION message is sent
 * to the application and the rest of the cluster needs to be discarded as a new plan for the exploration needs to be
 * determined and comunicated by the application
 */
bool handleMoveAction(compositeAction* action) {
  
  char* actionArgs = action->getArgs();
  enum object_movement direction = (actionArgs[0] - '0');

  //Make the robot face the requested direction of movement
  zumo_robot.faceDirection(direction);
  
  /* 
   * If the robot is still scanning it means it will not receive any PUSH command, so we
   * simply move in the direction given, checking if we find the block
   */
  if (zumo_robot.isScanning()) {
    
    bool foundBlock = zumo_robot.goAhead(action->getRepetition(), true);

    if (foundBlock) {
      
      // These two lines simply determines the position of the block: 1 cell ahead of the robot in the direction of the movement
      block block(DEFAULT_ID, zumo_robot.position);
      block.move(direction, 1);

      bluetooth.sendLocation(block.position, zumo_robot.position);

    }
    #ifdef ALWAYS_SEND_ROBOT_LOCATION
    else {
      bluetooth.sendLocationRobotOnly(zumo_robot.position);
    }
    #endif

    return foundBlock;
    
  }
  else {

    if (actionArgs[1] == '0') {
      zumo_robot.goAhead(action->getRepetition());
    }
    else if (actionArgs[1] == '1') {
      zumo_robot.pushBlock(action->getRepetition());
    }
    return false;
    
  }
}

void setup() {

  //Hardware initialization
  zumo_robot.hardwareInit();

# ifdef DEBUG_LINE_CALIBRATION
  for (int i = 0; i < 3; i++) {
    lcd.print(lineSensors.calibratedMinimumOn[i]);
    lcd.gotoXY(0,1);
    lcd.print(lineSensors.calibratedMaximumOn[i]);
    delay(2000);
    lcd.clear();
  }
# endif

# ifdef DEBUG_POSITION
  lcd.clear();
  lcd.print(zumo_robot.position.x);
  lcd.gotoXY(0, 1);
  lcd.print(zumo_robot.position.y);
# endif

  ledYellow(true);  // The led yellow indicates that we are in scan mode

}

void loop() {

  char actionType;
  char* actionArgs;
  compositeAction currentAction;

  bool foundBlock = false;
  uint16_t executedInstructions = 0;
  boolean clusterOk = bluetooth.receiveCluster();

  do {
    
    currentAction = bluetooth.getNextAction();
    actionType = currentAction.getType();
    actionArgs = currentAction.getArgs();

    switch (actionType) {
      case HEADER_TYPE_MOVE:    foundBlock = handleMoveAction(&currentAction);
                                break;
      
      case HEADER_TYPE_STATE:   if (actionArgs[0] == 'S') {
                                  zumo_robot.setScanMode();
                                }
                                else if (actionArgs[0] == 'E') {
                                  zumo_robot.setExecuteMode();
                                }
                                break;

      case HEADER_TYPE_END:     break;
    }

    executedInstructions += currentAction.getRepetition();
    
  } while (actionType != '\0' && !foundBlock);

  // If we haven't received a full cluster but we found the block we don't send any warning as we would have discarded the rest of the cluster anyway
  if (executedInstructions != bluetooth.getClusterLength() && !foundBlock) {

    // We doesn't send anything until the robot is contacted by the app first
    if (!bluetooth.isClusterEmpty()) {

      //Flushes the bluetooth buffer to discard remaining data
      delay(2000);
      while (Serial1.read() != -1) ;
      bluetooth.sendWarning(executedInstructions);
    }
    
  }
  // When the block is found we don't need to send DONE: it is sent only when a FULL cluster is executed 
  else if (!foundBlock) {
    bluetooth.sendDone();
  }
  
}
