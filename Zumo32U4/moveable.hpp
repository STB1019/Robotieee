/**
 * @file
 * 
 * Represents something that can move throughout robotieee::model::workplace
 *
 * @date Feb 19, 2018
 * @author koldar
 */

#ifndef MOVEABLE_HPP_
#define MOVEABLE_HPP_

#include <point.hpp>

using namespace robo_utils;

namespace robotieee {

/**
 * A possible direction the robot ( or a block) can logically move
 */
enum class object_movement {
    /**
     * something move up in the robotieee::model::workplace
     * 
     * This means we are approaching to row 0
     */
		UP = 1,
   /**
    * something moves down in the robotieee::model::workplace
    * 
    * This means we are approaching maximum row
    */
		DOWN = 2,
   /**
    * something moves to the right
    */
		RIGHT = 3,
    /**
     * something moves to the left
     */
		LEFT = 4
	};

class moveable {
public:
  /**
   * The position this object has within robotieee::model::workplace matrix
   */
	point position;
public:
    /**
     * Alter the position of a given object
     * 
     * @param[in] om the direction where the object should move
     */
		virtual void move(object_movement om);
    /**
     * Dispose the instance
     */
		virtual ~moveable() = 0;
    /**
     * Setup the instance
     * 
     * @param[in] p the position where the object is inside robotieee::model::workplace
     */
    virtual moveable(const point& p);
};

}


#endif /* MOVEABLE_HPP_ */
