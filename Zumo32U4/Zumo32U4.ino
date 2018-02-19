
#define AVR_BUILD
#include "model.hpp"

using namespace robo_utils;
using namespace robotieee;

static model zumo_model = model{10, 10};

void setup() {
  // put your setup code here, to run once:

  zumo_model.zumo_robot.move(object_movement::UP);
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
