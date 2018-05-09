#define AVR_BUILD
#define DEFAULT_ID 0 // Used as we do not care about blocks ids in this version of the code
#define EXEC_PROTOCOL_VERSION  2
#define ALWAYS_SEND_ROBOT_LOCATION

#include <Zumo32U4.h>
#include "robot.hpp"
#include "block.hpp"
#include "actionComunicator.hpp"
#include "compositeAction.hpp"
#include "ICommunicator.hpp"
#include "BluetoothAsSerial.hpp"

#include "string.hpp" //only for connection test

using namespace robo_utils;
using namespace robotieee;

L3G gyro;
LSM303 accel;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensors;
#if EXEC_PROTOCOL_VERSION == 1
  actionComunicator bluetooth;
#elif EXEC_PROTOCOL_VERSION >= 2
  ICommunicator* bluetooth = BluetoothAsSerial::getInstance(); 
#endif

#ifdef DEBUG_LCD
Zumo32U4LCD lcd;
#endif

// CARE: The robot needs to be placed in the upper-left corner of the grid and facing towards the bottom of the grid
// !!!!!! CARE !!!!!!
// THE POINT INITIALIZATION SYNTAX IS (Y,X)
robot zumo_robot{(point) {0, 0}};

#if EXEC_PROTOCOL_VERSION == 1
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

/*
This fuction is the activity execution for first protocol version.

*/
void doActivityVersion1() {

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
      //delay(2000);
      //while (Serial1.read() != -1) ;
      bluetooth.sendWarning(executedInstructions);
      ledRed(true);
      Zumo32U4Buzzer::playNote(NOTE_D(2), 500, 15);
      while(true) ;
    }
    
  }
  // When the block is found we don't need to send DONE: it is sent only when a FULL cluster is executed 
  else if (!foundBlock) {
    bluetooth.sendDone();
  }
}
#endif /* PROTOCOL_VERSION == 1 */

#if EXEC_PROTOCOL_VERSION >= 2
/*
This function manage the movement action and return the package to sent as responce to the host.
*/
IPackage* doMovement(IPackage* package, IMessage* action) {
  IPackage* responce = nullptr; 
  char* actionArgs = action->getArgs();
  enum object_movement direction = (actionArgs[0] - '0');

  //Make the robot face the requested direction of movement
  zumo_robot.faceDirection(direction);
  
  /* 
   * If the robot is still scanning it means it will not receive any PUSH command, so we
   * simply move in the direction given, checking if we find the block
   */
  if (zumo_robot.isScanning()) {
    
    bool foundBlock = zumo_robot.goAhead(1, true);

    if (foundBlock) {
      
      //TODO: edit the block position with the new proximity management
      // These two lines simply determines the position of the block: 1 cell ahead of the robot in the direction of the movement
      block block(DEFAULT_ID, zumo_robot.position);
      block.move(direction, 1);

      responce = BluetoothAsSerial::initFoundAck(package, zumo_robot.position, block.position);
    } else {
      responce = BluetoothAsSerial::initNotFoundAck(package, zumo_robot.position);
    }
  }
  else {

    if (actionArgs[1] == '0') {
      zumo_robot.goAhead(1);
    }
    else if (actionArgs[1] == '1') {
      zumo_robot.pushBlock(1);
    }
    
    responce = BluetoothAsSerial::initAcknowledge(package);
  }
  
  return responce;
}

/*
This fuction is the activity execution for second protocol version and on.
Uses the interfaces ICommunicator, IPackage and IMessage to separate the program logic from the protocol used.

NB: use directly the protocol implementation class only to init the package for the message to send.
*/
void doActivity() {
  bool foundBlock = false;
  IPackage* packageRead = new CommunicationPackage();
  IPackage* packageSent = nullptr;
  IMessage* packageMessage = new compositeAction();

  //led red on: package waiting
  ledRed(true);

  //wait for a package
  while(!bluetooth->waitForPackage(packageRead));

  //led red off: package received
  ledRed(false);

  //Exec only if the package is an instruction type
  if(packageRead->getType() == PACKAGE_TYPE_INSTRUCTION) {
    //Extract the message from the payload
    packageRead->getPayloadAsMessage(packageMessage);

    //Executing the message
    switch(packageMessage->getType()) {
      case MESSAGE_TYPE_MOVE:
          packageSent = doMovement(packageRead, packageMessage);
          /*//Init the package to send as ack
          if (foundBlock) {
            packageSent = BluetoothAsSerial::initFoundAck(packageRead, zumo_robot.position, block.position);
          } else {
            packageSent = BluetoothAsSerial::initNotFoundAck(packageRead, zumo_robot.position);
          }*/
          break;
      
      case MESSAGE_TYPE_STATE_CHANGE:
          if (packageMessage->getArgs()[0] == 'S') {
            zumo_robot.setScanMode();
          }
          else if (packageMessage->getArgs()[0] == 'E') {
            zumo_robot.setExecuteMode();
          }

          //Init the package to send as ack
          packageSent = BluetoothAsSerial::initAcknowledge(packageRead);
          break;
    }

    //sending the ack to host
    bluetooth->sendPackage(packageSent);
  }

  //Free memory
  delete(packageMessage);
  delete(packageRead);
  delete(packageSent);
}

void testConnectionClass() {
  CommunicationPackage* pSent = nullptr;
  CommunicationPackage* pRead = new CommunicationPackage();
  string<MAX_PAYLOAD_LENGTH> payload = string<MAX_PAYLOAD_LENGTH>{"Per me e' la cipolla"};

  BluetoothAsSerial::jumpByte(42); //length of 'Bluetooth device connected at 00:00:00.000' string sent by btTerminal on connection

  while(true) {
    //led red on: package waiting
    ledRed(true);

    //wait for a package
    while(!bluetooth->waitForPackage(pRead));

    //led red off: package received
    ledRed(false);

    delete(pSent);
    pSent = BluetoothAsSerial::initAcknowledge(pRead);
    //package is an ack -> no need responce from host
    bluetooth->sendPackage(pSent);
    
    delete(pSent);
    pSent = BluetoothAsSerial::initFoundAck(pRead, point(65,97), point(66,98));
    //package is an ack -> no need responce from host
    bluetooth->sendPackage(pSent);

    delete(pSent);
    pSent = BluetoothAsSerial::initNotFoundAck(pRead, point(65,97));
    //package is an ack -> no need responce from host
    bluetooth->sendPackage(pSent);

    delete(pSent);
    pSent = new CommunicationPackage(PROTOCOL_VERSION, 0x0, 20, PACKAGE_TYPE_COMMUNICATION);
    pSent->setPayload(&payload);
    //package isn't an ack -> robot will wait for the responce, if not received in 1 minute and half it will re-send the package
    bluetooth->sendPackage(pSent);
  }

  /*
  Test example:
  package to send in string: ' ç"CIl robot parla e dice: ciao mondo!'
  version: 2
  id: 1
  payload length: 34
  type: C
  payload: 'Il robot parla e dice: ciao mondo!'

  if the robot will receive it correctly will return:
  empty ack, string: ' ç' + '\0' + A' byte: 0x20 0x80 0x00 0x41 (version: 2; id: 1; pLength: 0; type: A)
  found ack, string: ' ç' + EOT + 'AaAbB' byte: 0x20 0x80 0x04 0x41 0x91 0x41 0x92 0x42 (version: 2; id: 1; pLength: 0; type: A; payload: aAbB)
  not found ack, string: ' ç' + EOT + 'AaA' byte: 0x20 0x80 0x04 0x41 0x91 0x41 (version: 2; id: 1; pLength: 0; type: A; payload: aA)

  the will send a package like 
  string: ' ' + '\0' + DC4 + 'CPer me e' la cipolla'
  byte: 0x20 0x00 0x14 0x43 0x50 0x65 0x72 0x20 0x6d 0x65 0x20 0x65 0x2c 0x20 0x6c 0x61 0x20 0x63 0x69 0x70 0x6f 0x6c 0x6c 0x61
  (version: 2; id: 0; pLength: 20; type: C; payload: Per me e' la cipolla)

  and expect an ack like:
  string: ' ' + '\0' + '\0' + 'A'
  byte: 0x20 0x00 0x00 0x41
  */
}

#endif /* PROTOCOL_VERSION >= 2 */

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

#ifdef DEBUG_PROXIMITY
  while(!buttonA.getSingleDebouncedPress()) {
      proxSensors.read();
      byte frontLeft = proxSensors.countsFrontWithLeftLeds();
      byte frontRight = proxSensors.countsFrontWithRightLeds();
      bluetooth.sendWarning(frontLeft);
      bluetooth.sendWarning(frontRight);
      delay(1000);
  }
#endif

  zumo_robot.setScanMode();
}

void loop() {

  #if EXEC_PROTOCOL_VERSION == 1
    doActivityVersion1();
  #elif EXEC_PROTOCOL_VERSION >= 2
    //doActivity();
    testConnectionClass();
  #endif

}
