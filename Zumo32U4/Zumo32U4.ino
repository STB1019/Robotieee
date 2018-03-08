 
#define AVR_BUILD

#include <Zumo32U4.h>
#include <Wire.h>
#include "model.hpp"

using namespace robo_utils;
using namespace robotieee;

L3G gyro;
LSM303 accel;
Zumo32U4LCD lcd;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4Motors motors;
int function;
int exit_switch = 0;
int rotation;
int distance = 0;

model zumo_model = model{10, 10};

void setup() {

  //Hardware initialization
  zumo_model.zumo_robot.hardwareInit();
  
  lcd.clear();
  lcd.print(F("Select"));
  lcd.gotoXY(0, 1);
  lcd.print(F("Funct."));

  delay(3000);

  lcd.clear();
  lcd.print(F("A)Rt/B)Mv"));
  lcd.gotoXY(0, 1);
  lcd.print(F("C)Test"));
}

void loop() { 
  
  while(exit_switch == 0){
    if(buttonA.getSingleDebouncedPress())
    {
      function = 1; //Rotation
      exit_switch = 1;
    }
    else if(buttonB.getSingleDebouncedPress())
    {
      function = 2; //Movement
      exit_switch = 1;
    }
    else if(buttonC.getSingleDebouncedPress())
    {
      function = 3; //End function
      exit_switch = 1;
    } 
    else
    {
      exit_switch=0;
    }
  }

  
  do {
    if (function == 1){
  
      lcd.clear();
      lcd.print(F("Select"));
      lcd.gotoXY(0, 1);
      lcd.print(F("Rotation"));
  
      delay(3000);
  
      lcd.clear();
      lcd.print(F("A)90 B)180"));
      lcd.gotoXY(0, 1);
      lcd.print(F("C)360"));
      
      exit_switch = 0;
      
      //Allows the user to choose how many degrees to rotate
      while (exit_switch == 0){
        if(buttonA.getSingleDebouncedPress())
        {
          rotation = 90;
          exit_switch = 1;
        }
        else if(buttonB.getSingleDebouncedPress())
        {
          rotation = 180;
          exit_switch = 1;
        }
        else if (buttonC.getSingleDebouncedPress())
        {
          rotation = 360;
          exit_switch = 1;
        } 
        else 
        {
          exit_switch = 0;
        }
      }
      delay(1000);
      
      //Calibrates the Gyroscope and then rotates.
      zumo_model.zumo_robot.calibrateGyroscope();
      buttonA.waitForButton();
      zumo_model.zumo_robot.rotate(rotation);
      function = 3;
    }
  
    else if(function == 2){
      
      lcd.clear();
      lcd.print(F("+10/-10"));
      lcd.gotoXY(0, 1);
      lcd.print(F("C)onfirm"));
      delay(1000);
      exit_switch = 0;

      //Allows the user to choose how many cms to move
      while (exit_switch == 0) {
        if(buttonA.getSingleDebouncedPress())
        {
          distance = distance + 10;
          lcd.clear();
          lcd.print(distance);
        }
        else if(buttonB.getSingleDebouncedPress())
        {
          distance = distance - 10;
          lcd.clear();
          lcd.print(distance);
        }
        else if(buttonC.getSingleDebouncedPress())
        {
          exit_switch=1;
        }
      };
      delay(1000);

      //Moves the robot
      zumo_model.zumo_robot.moveStraight(distance);
      function = 3;
    }
  
    else if (function == 3)
    {
      exit_switch = 0;
      lcd.clear();
      lcd.print(F("End"));
    }
  }while(function == 0);   
}
