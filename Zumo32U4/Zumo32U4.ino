#define AVR_BUILD

#include <Zumo32U4.h>
#include <Wire.h>
#include "model.hpp"

#include "compositeAction.hpp"
#include "actionComunicator.hpp"

using namespace robo_utils;
using namespace robotieee;

L3G gyro;
LSM303 accel;
Zumo32U4LCD lcd;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensors;

model zumo_model = model{10, 10};

void setup() {

  //Hardware initialization
  zumo_model.zumo_robot.hardwareInit();

  delay(1000);

  # ifdef DEBUG_LINE_CALIBRATION
    for (int i = 0; i < 3; i++) {
      lcd.print(lineSensors.calibratedMinimumOn[i]);
      lcd.gotoXY(0,1);
      lcd.print(lineSensors.calibratedMaximumOn[i]);
      delay(2000);
      lcd.clear();
    }
  # endif

  //Initial delay
  for (int i = 0; i < 3; i++) {
    lcd.clear();
    lcd.print(3 - i);
    delay(1000);
  }

  /*
  zumo_model.zumo_robot.pushBlock(1);
  zumo_model.zumo_robot.turnLeft();
  zumo_model.zumo_robot.goAhead(1,false);
  zumo_model.zumo_robot.turnRight();
  zumo_model.zumo_robot.goAhead(1,false);
  zumo_model.zumo_robot.turnRight();
  zumo_model.zumo_robot.pushBlock(1);
  */

  testComunication();
  //testDriveApp();

  #ifdef DEBUG_POSITION
  lcd.clear();
  lcd.print(zumo_model.zumo_robot.position.x);
  lcd.gotoXY(0, 1);
  lcd.print(zumo_model.zumo_robot.position.y);
  #endif

  
  #ifndef DEBUG
    lcd.clear();
    lcd.print(F("DONE!"));
  #endif
}

void loop() {
  
}

void testDriveApp(bool blinkMove) {
  actionComunicator comunicator;
  compositeAction action;
  bool fullCluster = true;

  comunicator.setClusterLength(1);

  ledGreen(true);
  delay(200);
  ledGreen(false);
  buttonA.waitForButton();
  comunicator.flush(); //clear buffer
  buttonA.waitForButton();
  
  while (true) {
    fullCluster = comunicator.receiveCluster();

    do {
      action = comunicator.getNextAction();

      if (action.getType() != '\0') {
        //Only action HEADER_TYPE_MOVE
        
        blinkMove ? testMoveBlink(action) : testMoveMove(action);

      }

    } while(action.getType() != '\0');

  } //end while(true)

}

void testMoveMove(compositeAction act) {
  bool notGo = false;
  uint8_t timeMovement = 200;
  /*
  first argument's digit:
    0 ->  go forward
    1 ->  turn right and go forward
    2 ->  turn back and go forward
    3 ->  turn left and go forward
  */

  ledYellow(true);

  switch(act.getArgs()[0]) {
    case '0':
      break;
    
    case '1':
      zumo_model.zumo_robot.turnRight();
      break;
    
    case '2':
      zumo_model.zumo_robot.turnBack();
      break;
    
    case '3':
      zumo_model.zumo_robot.turnLeft();
      break;

    default:
      notGo = true;
      break;
  }

  if (!notGo)
      zumo_model.zumo_robot.timeMove(timeMovement);
      
  ledYellow(false);
}

void testMoveBlink(compositeAction act) {
  uint8_t timeBlink = 1000;
  /*
  first argument's digit:
    0 ->  red,yellow,green led blink
    1 ->  red led blink
    2 ->  green led blink
    3 ->  yellow led blink
  */
  switch(act.getArgs()[0]) {
    case '0':
      ledGreen(true);
      ledYellow(true);
      ledRed(true);
      delay(timeBlink);
      ledGreen(false);
      ledYellow(false);
      ledRed(false);
      break;
    
    case '1':
      ledRed(true);
      delay(timeBlink);
      ledRed(false);
      break;
    
    case '2':
      ledGreen(true);
      delay(timeBlink);
      ledGreen(false);
      break;
    
    case '3':
      ledYellow(true);
      delay(timeBlink);
      ledYellow(false);
      break;

    default:
      break;
  }
}

void testComunication() {
  uint8_t timeBlink = 1000;
  actionComunicator comunicator;
  compositeAction action;
  bool fullCluster = true;
  
  comunicator.sendLocation(point(9,4),point(10,4));

  ledGreen(true);
  delay(200);
  ledGreen(false);
  buttonA.waitForButton();
  comunicator.flush(); //clear buffer
  buttonA.waitForButton();
  
  while (true) {
    fullCluster = comunicator.receiveCluster();

    do {
      action = comunicator.getNextAction();

      if (action.getType() != '\0') {
        for (int i = 0; i < action.getRepetition(); i++) {
          /*
            'M' blink yellow
            'S' blink red
            'E' blink green
            uknown blink all
          */
          switch(action.getType()) {
            case 'M':
              ledYellow(true);
              delay(timeBlink);
              ledYellow(false);
              break;
            case 'S':
              ledRed(true);
              delay(timeBlink);
              ledRed(false);
              break;
            case 'E':
              ledGreen(true);
              delay(timeBlink);
              ledGreen(false);
              break;
            default:
              ledGreen(true);
              ledYellow(true);
              ledRed(true);
              delay(timeBlink);
              ledGreen(false);
              ledYellow(false);
              ledRed(false);
              break;
          }

          delay(1000); //time between one action and another
        }
      }

    } while(action.getType() != '\0');

  } //end while(true)

  /*
  Serial.begin(9600);
  Serial.println("INIZIO");
  Serial1.println("INIZIO");
  do {
    action = comunicator.getNextAction();
    Serial1.print(action.getType());
    Serial1.print(action.getArgs());
    Serial1.print(action.getRepetition());
    if (action.getType() != '\0') {
      for(int k = 0; k < action.getRepetition(); k++) {
        switch(action.getType()) {
          case 'M':
            for(int i = 0; i < (int)(action.getArgs()[0] - '0'); i++) {
              ledYellow(true);
              delay(500);
              ledYellow(false);
            }
            break;
          case 'S':
            ledRed(true);
            delay(500);
            ledRed(false);

            if (action.getArgs()[0] == 'E') {
              ledRed(true);
              delay(500);
              ledRed(false);
            }
            break;
          case 'E':
            ledGreen(true);
            delay(500);
            ledGreen(false);
            break;
          default:
            ledGreen(true);
            ledYellow(true);
            ledRed(true);
            delay(500);
            ledGreen(false);
            ledYellow(false);
            ledRed(false);
            break;
        }

        delay(500);
      }
    }
    else {
      for(int i = 0; i < 2; i++) {
          ledGreen(true);
          ledYellow(true);
          ledRed(true);
          delay(500);
          ledGreen(false);
          ledYellow(false);
          ledRed(false);
          delay(500);
      }
    }
  } while(action.getType() != '\0');

  Serial1.println("FINE");

  if (!fullCluster) {
    comunicator.sendWarning();
  }
  else {
    comunicator.sendDone();
  }
  */
}