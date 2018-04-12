/**
 * @file
 * 
 * A moveable block in the sokoban domain
 *
 * @date Feb 19, 2018
 * @author koldar
 */

#ifndef block_HPP_
#define block_HPP_

#include "moveable.hpp"
#include "typedefs.hpp"

namespace robotieee {

/**
 * Represents a block that can be moved
 */
class block : public moveable {
private:
	/**
	 * An identifier which uniquely identifes the block
	 */
	id_t id;
public:
  /**
   * setup a new block
   * 
   * @param[in] id a number which uniquely identify a block
   * @param[in] start_position a point where the block is situation within the robotieee::model::workplace matrix
   */
	block(id_t id, const point& start_position);
  /**
   * copy constructor
   * 
   * \note
   * the id will be copied as well
   * 
   * param[in] b the block to copy
   */
	block(const block& b);
  /**
   * dispose the current block
   */
	~block();
public:
	void move(object_movement om);
};

}


#endif /* block_HPP_ */
