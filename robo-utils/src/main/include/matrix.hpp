#ifndef CPP_MATRIX__
#define CPP_MATRIX__

#include "point.hpp"

namespace robo_utils {

template <typename T>
class matrix {
private:
	T* _m;
	const unsigned int _rows;
	const unsigned int _columns;
public:
	matrix(unsigned int rows, unsigned int columns, const T& initialValue);
	~matrix();
	unsigned int rows() const;
	unsigned int columns() const;
	T& operator() (unsigned int row, unsigned int col);
	T  operator() (unsigned int row, unsigned int col) const;
	T& operator() (const point& p);
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
