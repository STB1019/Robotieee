/*
 * abstract_list.hpp
 *
 *  Created on: Feb 19, 2018
 *      Author: koldar
 */

#ifndef ABSTRACT_LIST_HPP_
#define ABSTRACT_LIST_HPP_

namespace robo_utils {

template<typename T>
class abstract_list {
public:
	/**
	 * Adds an element at the end of the list
	 *
	 * @param[in] el the element to add to the tail of the list
	 */
	virtual void add_to_tail(const T el) = 0;
	/**
	 * Adds an element at the beginning of the list
	 *
	 * @param[in] el the element to add to the ehad of the list
	 */
	virtual void add_to_head(const T el) = 0;
	/**
	 * Get the i-th element of the list
	 *
	 * @param[in] index the index of the element to fetch
	 * @return
	 * 	\li the element in the list;
	 * 	\li the default value if \c index leads to no item
	 */
	virtual T get(int index) = 0;
	/**
	 * Get the first element of the list
	 *
	 * @return
	 * 	\li the head of the list;
	 * 	\li the default value if the list is empty
	 */
	virtual T get_head() = 0;
	/**
	 * Get the last element of the lsit
	 *
	 * @return
	 * 	\li the last element of the list;
	 * 	\li the default value if the list is empty
	 */
	virtual T get_tail() = 0;
	/**
	 * Check if the list is empty
	 *
	 * @return \c true if the list is empty; \c false otherwise
	 */
	virtual bool is_empty() = 0;
	/**
	 * The number of elements
	 *
	 * @return the number of elements within the list
	 */
	virtual int get_size() = 0;
	/**
	 * Remove the head of the list
	 *
	 * \post
	 * 	\li the first item of the list will be updated
	 *
	 * @return
	 * 	\li the head of the list;
	 * 	\li the default value if the list is empty
	 */
	virtual T pop_head() = 0;
public:
	/**
	 * A lvaue pointing to the i-th element of the list
	 *
	 * You can use this function to alter the i-th value of the list
	 *
	 * @code
	 * 	list<int> l{0, false};
	 * 	l.add_tail(1);
	 * 	l.add_tail(2);
	 * 	l.add_tail(3);
	 * 	l[1] = 5 //2 becomes 5
	 * @endcode
	 *
	 * @param[in] i the index of the cell to update
	 * @return the lvalue representing the cell to (possibly) change
	 */
	virtual T& operator[](unsigned int i) = 0;
	/**
	 * A constant lvalue to the i-th element of the list
	 *
	 * @param[in] i the index of the cell to update
	 * @return the lvalue representing the cell to (possibly) change
	 */
	virtual T operator[](unsigned int i) const = 0;
public:
	virtual ~abstract_list() = 0;
};

template<typename T>
abstract_list<T>::~abstract_list() {
}

}

#endif /* ABSTRACT_LIST_HPP_ */
