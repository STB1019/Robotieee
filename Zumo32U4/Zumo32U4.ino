#include <fixed_list.hpp>

using namespace robo_utils;

void setup() {
  // put your setup code here, to run once:

  abstract_list<int>* al = new fixed_list<int>{0, 10};
}

void loop() {
  // put your main code here, to run repeatedly:

}
