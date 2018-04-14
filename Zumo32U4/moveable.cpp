/*
 * moveable.cpp
 *
 *  Created on: Feb 19, 2018
 *      Author: koldar
 */

#include "moveable.hpp"

namespace robotieee {

void moveable::move(enum object_movement om, unsigned int n) {
	switch (om) {
	case object_movement::DOWN: {
		this->position.y -= n;
		break;
	}
	case object_movement::UP: {
		this->position.y += n;
		break;
	}
	case object_movement::LEFT: {
		this->position.x += n;
		break;
	}
	case object_movement::RIGHT: {
		this->position.x -= n;
		break;
	}
	}
}

moveable::moveable(const point& p) : position{p} {
  
}

moveable::~moveable() {

}

}
