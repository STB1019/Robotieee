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
};

}

#endif /* ROBOT_HPP_ */
