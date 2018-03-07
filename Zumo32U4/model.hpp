/**
 * @file
 * 
 * Specify the model used by the robot to represent the physical world and its relations
 * 
 * @author koldar
 * @date 19 Feb 2018
 */

#ifndef MODEL_HPP_
#define MODEL_HPP_

#define AVR_BUILD

#include <Zumo32U4.h>
#include <list.hpp>
#include <matrix.hpp>
#include <point.hpp>
#include "block.hpp"
#include "robot.hpp"

/**
 * A point used to place something in robiteee::model::workplace in a *I don't care* position
 */
#define DEFAULT_POINT {-1, -1}
/**
 * Represents the maximum id of a bit in robotieee::base_cell_content which is meaningful
 */
#define CELL_CONTENT_MAX_SIZE 4
/**
 * Represents the content of a cell in robotieee::model::workplace which doesn't contain anything
 */
#define EMPTY_CELL 0


namespace robotieee {

/**
 * A cell may contain several things at once
 * Every value of this enumeration is interpreted as the index of the bit in an int.
 * If the i-th bit in the given \c int is set, the concepts whose value is said \c i occurs in the cell
 * represneted by said \c int.
 */
enum base_cell_content {
  /**
   * bit index of the robot
   */
  BCC_PLAYER =      0,
  /**
   * bit index of a block
   */
  BCC_BLOCK =       1,
  /**
   * bit index of a goal where a block should be positioned
   */
  BCC_GOAL =        2,
  /**
   * bit index of a docking station
   */
  BCC_DOCKING_STATION = 3,
  /**
   * bit index of a unvalicable cell.
   *
   * It should be mutually exclusive with anything else
   */
  BCC_OBSTRUCTED =    CELL_CONTENT_MAX_SIZE
};

/**
 * The type of a cell in robotieee::model::workplace
 */
typedef unsigned int cell_content;

	typedef list<point> point_list;

	class const_base_cell_content_iter;


	class model {
		friend class const_base_cell_content_iter;
	  private:
    /**
     * represent the grid where the sumo robot is performing its sokoban work
     */
		matrix<cell_content> workplace;
    /**
     * the position, within robotieee::model::workspace , where the docking station of the robot is located
     */
		point docking_station;
    /**
     * The list of all blocks in robotieee::model::workspace discovered so far
     */
		point_list blocks;
    /**
     * The list of places where the goals are
     * 
     * Goals are position where each block should (at the end of the plan computed by the planning system) be positioned
     * 
     * There are as many goals as there are blocks
     */
		point_list goals;
	  public:
    /**
     * A variable representing the robot itself
     */
		robot zumo_robot;
	  private:
		bool has_cell_property(unsigned int y, unsigned int x, enum base_cell_content object) const;
		void set_cell_property(unsigned int y, unsigned int x, enum base_cell_content object, bool enable);
		void mark_cell_property(unsigned int y, unsigned int x, enum base_cell_content object);
		void unmark_cell_property(unsigned int y, unsigned int x, enum base_cell_content object);
		void toggle_cell_property(unsigned int y, unsigned int x, enum base_cell_content object);
		bool has_cell_property(const point& p, enum base_cell_content object) const;
		void set_cell_property(const point& p, enum base_cell_content object, bool enable);
		void mark_cell_property(const point& p, enum base_cell_content object);
		void unmark_cell_property(const point& p, enum base_cell_content object);
		void toggle_cell_property(const point& p, enum base_cell_content object);
	  public:
		model(unsigned int maxRows, unsigned int maxCols);
		~model();
	  public:
		bool is_cell_empty(unsigned int y, unsigned int x) const;
		bool is_cell_empty(const point& p) const;
		const_base_cell_content_iter cbegin(const point& p) const;
		const_base_cell_content_iter cend(const point& p) const;

	};

		class const_base_cell_content_iter {
			friend class model;
			private:
				const model& involved_model;
				const point& involved_point;
				int current_bit;
			private:
				void compute_next_bit_set();
			public:
				const_base_cell_content_iter(const model& m, const point& p);
				const_base_cell_content_iter(const model& m, const point& p, int current_bit);
				~const_base_cell_content_iter();
			public:
				bool operator ==(const const_base_cell_content_iter& other) const;
				bool operator !=(const const_base_cell_content_iter& other) const;
				enum base_cell_content operator*() const;
				const_base_cell_content_iter& operator++();
		};


}

#endif
