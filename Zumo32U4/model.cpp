#include "model.hpp"
#include "bits.hpp"

namespace robotieee {

#define CELL_CONTENT_ITER_FINISHED -2

model::model(unsigned int maxRows, unsigned int maxCols) : workplace{maxRows, maxCols, EMPTY_CELL}, zumo_robot{DEFAULT_POINT, &workplace}, docking_station{0, 0}, blocks{DEFAULT_POINT, false}, goals{DEFAULT_POINT, false} {

}

model::~model() {

}

bool model::is_cell_empty(unsigned int y, unsigned int x) const {
	return this->workplace(y,x) == 0;
}

bool model::has_cell_property(unsigned int y, unsigned int x, enum base_cell_content object) const {
	return read_bit(this->workplace(y,x), object);
}

void model::set_cell_property(unsigned int y, unsigned int x, enum base_cell_content object, bool enable) {
	if (enable) {
		set_bit(this->workplace(y,x), (unsigned int) object);
	} else {
		clear_bit(this->workplace(y,x), object);
	}
}

void model::mark_cell_property(unsigned int y, unsigned int x, enum base_cell_content object) {
	this->set_cell_property(y, x, object, true);
}

void model::unmark_cell_property(unsigned int y, unsigned int x, enum base_cell_content object) {
	this->set_cell_property(y, x, object, false);
}

void model::toggle_cell_property(unsigned int y, unsigned int x, enum base_cell_content object) {
	toggle_bit(this->workplace(y,x), object);
}

bool model::is_cell_empty(const point& p) const {
	return this->workplace(p) == 0;
}

bool model::has_cell_property(const point& p, enum base_cell_content object) const {
	return this->has_cell_property(p.y, p.x, object);
}

void model::set_cell_property(const point& p, enum base_cell_content object, bool enable) {
	this->set_cell_property(p.y, p.x, object, enable);
}

void model::mark_cell_property(const point& p, enum base_cell_content object) {
	this->mark_cell_property(p.y, p.x, object);
}

void model::unmark_cell_property(const point& p, enum base_cell_content object) {
	this->unmark_cell_property(p.y, p.x, object);
}

void model::toggle_cell_property(const point& p, enum base_cell_content object) {
	this->toggle_cell_property(p.y, p.x, object);
}

const_base_cell_content_iter model::cbegin(const point& p) const {
	return const_base_cell_content_iter{*this, p};
}

const_base_cell_content_iter model::cend(const point& p) const {
	return const_base_cell_content_iter{*this, p, CELL_CONTENT_ITER_FINISHED};
}

const_base_cell_content_iter::const_base_cell_content_iter(const model& m, const point& p) : involved_model{m}, involved_point{p}, current_bit{-1} {
	this->compute_next_bit_set();
}

const_base_cell_content_iter::const_base_cell_content_iter(const model& m, const point& p, int current_bit) : involved_model{m}, involved_point{p}, current_bit{current_bit} {

}

const_base_cell_content_iter::~const_base_cell_content_iter() {

}

void const_base_cell_content_iter::compute_next_bit_set() {
	for (auto i=(this->current_bit+1); i<(CELL_CONTENT_MAX_SIZE+1); i++) {
		if (read_bit(this->involved_model.workplace(this->involved_point), i)) {
			this->current_bit = i;
			return;
		}
	}
	this->current_bit = CELL_CONTENT_ITER_FINISHED;
}

bool const_base_cell_content_iter::operator ==(const const_base_cell_content_iter& other) const {
	if (this == &other) {
		return true;
	}
	return
			(&(this->involved_model) == &(other.involved_model))  &&
			(this->involved_point == other.involved_point) &&
			(this->current_bit == other.current_bit);
}

bool const_base_cell_content_iter::operator !=(const const_base_cell_content_iter& other) const {
	return !(*this == other);
}

enum base_cell_content const_base_cell_content_iter::operator*() const {
	return (enum base_cell_content) this->current_bit;
}

const_base_cell_content_iter& const_base_cell_content_iter::operator++() {
	this->compute_next_bit_set();
	return *this;
}

}
