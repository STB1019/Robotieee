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
   * Initializes the robot given its starting position in the grid. Default values are used for movement parameters
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
   * After rotation the robot will check for a block on the next cell.
   * 
   * \note 
   *    \li Before calling this function for the first time, robotieee::robot::harwareInit must have been already run
   * 
   * @param[in] degrees The amount of desired rotation in degrees
   */
  bool rotateAndCheck(int16_t degrees);
  
  /**
   * Rotates the robot a given amount of degrees. Positive values represent counter clockwise rotations,
   * while negative values represent clockwise ones.
   * 
   * \note 
   *    \li Before calling this function for the first time, robotieee::robot::harwareInit must have been already run
   * 
   * @param[in] degrees The amount of desired rotation in degrees
   * @param[in] stopIfCenterBlack If true, the rotation will prematurely terminate if the center line sensor finds a black surface
   */
  bool rotate(int16_t degrees, bool stopIfCenterBlack = false);

  /**
   * Rotates the robot 90 degrees clockwise
   * 
   * \note 
   *    \li Before calling this function for the first time, robotieee::robot::harwareInit must have been already run 
   */
  void turnRight();

  /**
   * Rotates the robot 180 degrees
   * 
   * \note 
   *    \li Before calling this function for the first time, robotieee::robot::harwareInit must have been already run
   */
  void turnBack();

  /**
   * Rotates the robot 90 degrees counter-clockwise
   * 
   * \note 
   *    \li Before calling this function for the first time, robotieee::robot::harwareInit must have been already run
   */
  void turnLeft();

  /**
   * Makes the robot go through a given number of cells.
   * 
   * \note 
   *    \li Before calling this function for the first time, robotieee::robot::harwareInit must have been already run
   * 
   * @param[in] cells The number of cells to go through
   */
  void goAhead(unsigned int cells);

  /**
   * Make the robot folow a black line while checking for a block in the following cell
   * 
   *    * \note 
   *    \li Before calling this function for the first time, robotieee::robot::harwareInit must have been already run
   * 
   * \pre
   *    \li the center line sensor is on a black track;
   *    
   * @return true: block found; false: no block on the route
   */
  bool followLineAndCheck();
  
  /**
   * Make the robot folow a black line
   * 
   * \note 
   *    \li Before calling this function for the first time, robotieee::robot::harwareInit must have been already run
   * 
   * \pre
   *    \li the center line sensor is on a black track;
   * 
   * @param[in] searchBlock Flag to activate the block searching routine while following the black line
   * @return true: block found; false: no block on the route
   */
  void followLine();

  /**
   * Initializes, configures and calibrates when needed the hardware of
   * the Zumo32U3 robot.
   * \note This function needs to be called explicitly before calling any
   * hardware related function of the robotieee::robot class
   */
  void hardwareInit();

  /**
   * Sets the speed to be used by every future movement-related function
   * 
   * @param[in] speed The speed to be used for robot movement
   */
  void setSpeed(uint16_t speed);

 
private:
  bool _hardwareInitialized;          // Used to avoid multiple hardware initializations.
  uint16_t _speed;                    // The speed to be used by the robot in both rotations and straight movement. This values must be in range [-400, 400]
  uint16_t _centeringDelay;           // The amount of milliseconds to wait after finding an intersection. This is needed to center the robot on the cross
  uint8_t _pathSeekCompensation;      // The initial number of degrees to rotate when the robot is searching the lost black line. See fixPath function
  uint8_t _speedCompensation;         // The speed increase used to make the robot slightly rotate when it arrives at an intersection but it is not parallel to it
  //matrix<cell_content>* _gridCells;   // A pointer to the grid of cells. This is used to avoid the need to pass it as a parameter to most movement functions
  /**
   * This function is used internally by the other robot methods to adjust its trajectory
   * when an error is detected.
   */
  void fixPath();

  /**
   * This fuction is used internally by the other robot methods to check, with proximity sensors,
   * if there is a block in front of the robot with the nearest level.
   */
  bool checkForBlock();
  
  /**
   * This function is used to manually calibrate the Zumo32U4 line sensors.
   * This is done by first moving the robot manually on a light surface and
   * then on a dark one.
   */
  void calibrateLineSensors();
};

}

#endif /* ROBOT_HPP_ */

  bool followLine(bool searchBlock = false);