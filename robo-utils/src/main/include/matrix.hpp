/**
 * @file
 *
 * Provides a simple API for handling a matrix of objects
 *
 * @author koldar
 * @date 18 Feb 2018
 */

#ifndef CPP_MATRIX__
#define CPP_MATRIX__

#include "point.hpp"

namespace robo_utils {

/**
 * Represents a matrix of objects
 */
template <typename T>
class matrix {
private:
	/**
	 * Pointer to the first cell of the matrix
	 */
	T* _m;
	/**
	 * the number of rows of the matrix
	 */
	const unsigned int _rows;
	/**
	 * the number of column of the matrix
	 */
	const unsigned int _columns;
public:
	/**
	 * Initialize a new matrix
	 *
	 * @param[in] rows the number of rows the matrix has
	 * @param[in] columns the number of columns the matrix has
	 * @param[in] initialValue the value each cell of the matrix will have
	 */
	matrix(unsigned int rows, unsigned int columns, const T& initialValue);
	/**
	 * Dispose the matrix
	 */
	~matrix();
	/**
	 * the number of rows of the matrix
	 *
	 * @return the number of rows of the matrix
	 */
	unsigned int rows() const;
	/**
	 * the number of columns of the matrix
	 *
	 * @return the number of columns of the matrix
	 */
	unsigned int columns() const;
	/**
	 * Operator used to change a matrix cell
	 *
	 * @code
	 * matrix m<int>{3,3,0};
	 * m(1,2) = 5;
	 * @endcode
	 *
	 * @param[in] row the id of the row of the cell to change
	 * @param[in] col the id of the row of the cell to change
	 * @return the reference of the cell to change
	 */
	T& operator() (unsigned int row, unsigned int col);
	/**
	 * Operator used to get the value of a matrix cell
	 *
	 * @code
	 * matrix m<int>{3,3,4};
	 * std::cout << m(1,2) << std::endl;
	 * @endcode
	 *
	 * @param[in] row the id of the row of the cell to change
	 * @param[in] col the id of the row of the cell to change
	 * @return the value of the cell to change
	 */
	T  operator() (unsigned int row, unsigned int col) const;
	/**
	 * like matrix<T>::operator()(unsigned int row, unsigned int col) but accepts a point
	 *
	 * @param[in] p the point of the cell to change
	 * @return the reference of the cell to change
	 */
	T& operator() (const point& p);
	/**
	 *  like matrix<T>::operator()(unsigned int row, unsigned int col) const but accept a point
	 *
	 *  @param[in] p the point of the cell to change
	 *  @return the value of the cell to change
	 */
	T  operator() (const point& p) const;
};

#ifdef DESKTOP_BUILD
#include <stdlib.h>
#elif AVR_BUILD
#else
#error "Either DEKSTOP_BUILD or AVR_BUILD must be set"
#endif

template <typename T>
matrix<T>::matrix(unsigned int rows, unsigned int columns, const T& initialValue) : _rows(rows), _columns(columns), _m(nullptr){
	this->_m = (T*) calloc(rows * columns, sizeof(T));
	for (int y=0; y<this->_rows; y++) {
		for (int x=0; x<this->_columns; x++) {
			this->_m[y*this->_columns + x] = initialValue;
		}
	}
}

template <typename T>
matrix<T>::~matrix() {
	free(this->_m);
}

template <typename T>
T& matrix<T>::operator() (unsigned int row, unsigned int col) {
	return this->_m[row*this->_columns + col];
}

template <typename T>
T matrix<T>::operator() (unsigned int row, unsigned int col) const {
	return this->_m[row*this->_columns + col];
}
template <typename T>
T& matrix<T>::operator() (const point& p) {
	return this->_m[p.y*this->_columns + p.x];
}

template <typename T>
T  matrix<T>::operator() (const point& p) const {
	return this->_m[p.y*this->_columns + p.x];
}

template <typename T>
unsigned int matrix<T>::rows() const {
	return this->_rows;
}

template <typename T>
unsigned int matrix<T>::columns() const {
	return this->_columns;
}

}

#endif
