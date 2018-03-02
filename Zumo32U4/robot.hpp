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
   */
  void rotate(int16_t degrees);

  /**
   * Computes the average angular rate measured by the gyroscope during rotations.
   * In particular, the robot rotates until a fixed amount of measurements are taken. 
   * Once done, the robot will use the computed values to approximatively determine the 
   * amount of time needed for a rotation of a given angle.
   * 
   * @see robotieee::robot::rotate
   * 
   * \note Before calling robotieee::robot::rotate for the first time, you MUST call this function at least once.
   */
  void calibrateGyroscope();

private:
  static const int MOTORS_POWER = 150;
  static const int MOVEMENT_DELAY = 500;
  static const int GYRO_CALIBRATION_MEASUREMENTS = 10000;
  static const float GYRO_SENSITIVITY = 0.008750f;
  
  float _averageAngularRateCcw;
  float _averageAngularRateCw;
};

}

#endif /* ROBOT_HPP_ */
