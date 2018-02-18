/*
 * point.cpp
 *
 *  Created on: Feb 16, 2018
 *      Author: koldar
 */

#include "point.hpp"

namespace robo_utils {

point::point(int y, int x) : y{y}, x{x} {

}

point::point(point& p) : y{p.y}, x{p.x} {

}

point::~point() {

}

bool point::operator ==(const point& other) const {
	if (this == &other) {
		return true;
	}
	return (this->x == other.x) && (this->y == other.y);
}

bool point::operator !=(const point& other) const {
	return !(*this == other);
}

point& point::operator +=(const point& q) {
	this->x += q.x;
	this->y += q.y;
	return *this;
}

point& point::operator -=(const point& q) {
	this->x -= q.x;
	this->y -= q.y;
	return *this;
}

point& point::operator +=(int v) {
	this->x += v;
	this->y += v;
	return *this;
}

point& point::operator -=(int v) {
	this->x -= v;
	this->y -= v;
	return *this;
}

point point::operator +() const {
	point p{+this->x, +this->y};
	return p;
}

point point::operator -() const {
	point p{-this->x, -this->y};
	return p;
}

point operator +(point p, const point& q) {
	p += q;
	return p;
}

point operator -(point p, const point& q) {
	p -= q;
	return p;
}

point operator +(point p, int v) {
	p += v;
	return p;
}

point operator -(point p, int v) {
	p -= v;
	return p;
}

}
