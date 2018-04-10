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
#define AVR_BUILD

#include <Zumo32U4.h>
#include <matrix.hpp>
#include "moveable.hpp"
#include "typedefs.hpp"

namespace robotieee {

enum line_sensors {
    LEFT_SENSOR = 0,
    CENTER_SENSOR = 1,
    RIGHT_SENSOR = 2,
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
   * @param[in] start_position The initial position of the robot in the grid
   * @param[in] grid A pointer to the matrix representing the grid
   */
	robot(const point start_position, const matrix<cell_content>* grid);
 
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
   * 
   * @return Result of the check for a block at the end of rotation
   */
  bool turnRightAndCheck();

  /**
   * Rotates the robot 180 degrees
   * 
   * \note 
   *    \li Before calling this function for the first time, robotieee::robot::harwareInit must have been already run
   * 
   * @return Result of the check for a block at the end of rotation
   */
  bool turnBackAndCheck();

  /**
   * Rotates the robot 90 degrees counter-clockwise
   * 
   * \note 
   *    \li Before calling this function for the first time, robotieee::robot::harwareInit must have been already run
   * 
   * @return Result of the check for a block at the end of rotation
   */
  bool turnLeftAndCheck();

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
   * @param[in] lookingForBlocks If true, the function will stop early if a block is found
   * @return True if the robot stopped after finding a block, false otherwise
   */
  bool goAhead(unsigned int cells, bool lookingForBlocks = false);

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
  bool followLine(bool searchBlock = false);

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

  /**
   * Inverts the speed to be used by every future movement-related function
   */
  void invertSpeed();

  /**
   * Sets the value of the centering delay needed in future movement-related functions
   */
  void setCenteringDelay(uint16_t centeringDelay);

  /**
   * Moves the robot for a given amount of time
   * 
   * @param[in] delayMillis The amount of time for the movement in milliseconds
   */
  void timeMove(uint16_t time);

  /**
   * Pushes the block forwards by a given amount of cells
   * 
   * @param[in] cells The number of cells to push the block
   */
  void pushBlock(unsigned int cells);
 
private:
  bool _hardwareInitialized;          // Used to avoid multiple hardware initializations.
  int16_t _speed;                     // The speed to be used by the robot in both rotations and straight movement. This values must be in range [-400, 400]
  uint16_t _centeringDelay;           // The amount of milliseconds to wait after finding an intersection. This is needed to center the robot on the cross
  uint8_t _pathSeekCompensation;      // The initial number of degrees to rotate when the robot is searching the lost black line. See fixPath function
  uint8_t _speedCompensation;         // The speed increase used to make the robot slightly rotate when it arrives at an intersection but it is not parallel to it
  uint16_t _blockCenteringDelay;      // The amount of milliseconds do wait after finding an intersection. This is needed to center the block on the cross after pushing it
  matrix<cell_content>* _grid;        // A pointer to the grid of cells. This is used to avoid the need to pass it as a parameter to most movement functions
  enum object_movement _orientation;  // The direction that the robot is facing
  
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
