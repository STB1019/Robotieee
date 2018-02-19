/*
 * moveable.cpp
 *
 *  Created on: Feb 19, 2018
 *      Author: koldar
 */


#include "moveable.hpp"

namespace robotieee {

void moveable::move(object_movement om) {
	switch (om) {
	case object_movement::DOWN: {
		this->position.y -= 1;
		break;
	}
	case object_movement::UP: {
		this->position.y += 1;
		break;
	}
	case object_movement::LEFT: {
		this->position.x -= 1;
		break;
	}
	case object_movement::RIGHT: {
		this->position.x += 1;
		break;
	}
	}
}

moveable::moveable(const point& p) : position{p} {
  
}

moveable::~moveable() {

}

}
