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
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensors;

model zumo_model = model{10, 10};

void setup() {

  //Hardware initialization
  zumo_model.zumo_robot.hardwareInit();

  delay(1000);

  # ifdef DEBUG
    for (int i = 0; i < 3; i++) {
      lcd.print(lineSensors.calibratedMinimumOn[i]);
      lcd.gotoXY(0,1);
      lcd.print(lineSensors.calibratedMaximumOn[i]);
      delay(2000);
      lcd.clear();
    }
  # endif

  delay(5000);

  zumo_model.zumo_robot.goAhead(3);
  zumo_model.zumo_robot.pushBlock(3); 

  
  #ifndef DEBUG
    lcd.clear();
    lcd.print(F("DONE!"));
  #endif
}

void loop() {
  
}
