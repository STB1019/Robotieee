 /**
 * @file
 * 
 * API specifying a class representing the Zumo robot itself
 *
 * @date Feb 19, 2018
 * @author koldar
 */

#ifndef ROBOT_HPP_
#define ROBOT_HPP_

#include <Zumo32U4.h>
#include "moveable.hpp"

namespace robotieee {

enum line_sensors {
    LEFT = 0,
    CENTER = 1,
    RIGHT = 2,
  };

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
  enum line_color left;
  enum line_color center;
  enum line_color right;
};

/**
 * Represents Zumo32U4 robot itself
 */
class robot : public moveable {

public:

  
  /**
   * initialize a robot
   * 
   * @param[in] start_position the position where the robot is in robotieee::model::workplace
   */
	robot(const point& start_position);
 
 /**
  * dispose the robot
  */
	~robot();

  /**
   * Rotates the robot a given amount of degrees. Positive values represent counter clockwise rotations,
   * while negative values represent clockwise ones.
   * 
   * \note Before calling this function for the first time, you MUST call robotieee::robot::calibrateGyroscope at least once
   * 
   * @param[in] degrees The amount of desired rotation in degrees
   * @param[in] speed The speed to be used for the rotation
   * @param[in] stopIfCenterBlack If true, the rotation will prematurely terminate if the center line sensor finds a black surface
   */
  bool rotate(int16_t degrees, unsigned int speed, bool stopIfCenterBlack = false);

  /**
   * Make the robot folow a black line
   * 
   * \pre
   *  \li the center line sensor is on a black track;
   *  
   * @param[in] speed the speed the robot needs to have when following the line
   * @param[in] delta the amount of speed to increase or decrease if the robot is going out of trail
   */
  void followLine(unsigned int speed, unsigned int delta);

  /**
   * Initializes, configures and calibrates when needed the hardware of
   * the Zumo32U3 robot.
   * \note This function needs to be called explicitly before calling any
   * hardware related function of the robotieee::robot class
   */
  void hardwareInit();

 
private:

  /**
   * This function is used internally by the other robot methods to adjust its trajectory
   * when an error is detected.
   * 
   * @param[in] speed The speed to be used by the motors for moving
   * @param[in] delta A parameter determining how fast the robot should increase its attempts to find the way back
   */
  void fixPath(unsigned int speed, unsigned int delta);

  /**
   * This function is used to manually calibrate the Zumo32U4 line sensors.
   * This is done by first moving the robot manually on a light surface and
   * then on a dark one.
   */
  void calibrateLineSensors();
};

}

#endif /* ROBOT_HPP_ */
