/*
 * fixed_list.hpp
 *
 *  Created on: Feb 19, 2018
 *      Author: koldar
 */

#ifndef FIXED_LIST_HPP_
#define FIXED_LIST_HPP_

#include "abstract_list.hpp"

namespace robo_utils {

template <typename T>
class fixed_list_iter;

template<typename T>
class fixed_list : abstract_list<T>{
	friend class fixed_list_iter<T>;
private:
	const unsigned int capacity;
	unsigned int size;
	const T defaultValue;
	T* array;
public:
	fixed_list(T defaultValue, unsigned int capacity);
	~fixed_list();
private:
	bool add_at(int index, const T el);
public:
	void add_to_head(const T el);
	void add_to_tail(const T el);
	T get(int index);
	T get_head();
	T get_tail();
	bool is_empty();
	int get_size();
	T pop_head();
	T& operator[](unsigned int i);
	T operator[](unsigned int i) const;
public:
	fixed_list_iter<T> cbegin();
	fixed_list_iter<T> cend();
	fixed_list_iter<T> begin();
	fixed_list_iter<T> end();
	void remove_element(fixed_list_iter<T>& it);
	void insert_element(fixed_list_iter<T>& it, const T el);
};

template <typename T>
struct fixed_list_iter {
	friend class fixed_list<T>;
private:
	fixed_list<T>& container;
	int current_cell_index;
public:
	fixed_list_iter(fixed_list<T>& l, int starting_value);
	~fixed_list_iter();
public:
	bool operator ==(const fixed_list_iter<T>& other);
	bool operator !=(const fixed_list_iter<T>& other);
	T& operator*();
	fixed_list_iter<T>& operator++();
};

// ******************************* ITERATOR ************************************

template <typename T>
fixed_list_iter<T>::fixed_list_iter(fixed_list<T>& l, int starting_value) :
container{l}, current_cell_index{starting_value} {
}

template <typename T>
fixed_list_iter<T>::~fixed_list_iter() {
}

template <typename T>
bool fixed_list_iter<T>::operator ==(const fixed_list_iter<T>& other) {
	if (this == &other) {
		return true;
	}
	return (
			&this->container == &other.container &&
			this->current_cell_index == other.current_cell_index
	);
}

template <typename T>
bool fixed_list_iter<T>::operator !=(const fixed_list_iter<T>& other) {
	return !(*this == other);
}

template <typename T>
T& fixed_list_iter<T>::operator *() {
	return this->container.array[this->current_cell_index];
}

template <typename T>
fixed_list_iter<T>& fixed_list_iter<T>::operator++() {
	this->current_cell_index++;
	return *this;
}

// ****************************** FIXED LIST IMPLEMENTATION *****************************

template <typename T>
fixed_list<T>::fixed_list(T defaultValue, unsigned int capacity) : capacity{capacity}, size{0}, defaultValue{defaultValue}, array{nullptr} {
	this->array = new T[this->capacity];
}

template <typename T>
fixed_list<T>::~fixed_list() {
	delete [] this->array;
}

template <typename T>
bool fixed_list<T>::add_at(int index, const T el) {
	if (this->size == this->capacity) {
		//maximum capacity reached
		return false;
	}
	if (index > this->size) {
		return false;
	}
	if (index < this->size) { //append in the middle
		for (int i=(this->size-1); i>=index; i--) {
			this->array[i+1] = this->array[i];
		}
	}
	//if index == size we are adding on tail. index can't be greater than size
	this->array[index] = el;
	this->size += 1;
	return true;
}
template <typename T>
void fixed_list<T>::add_to_head(const T el) {
	this->add_at(0, el);
}

template <typename T>
void fixed_list<T>::add_to_tail(const T el) {
	this->add_at(this->size, el);
}

template <typename T>
T fixed_list<T>::get(int index) {
	return index < this->size ? this->array[index] : this->defaultValue;
}

template <typename T>
T fixed_list<T>::get_head() {
	return this->size > 0 ? this->array[0] : this->defaultValue;
}

template <typename T>
T fixed_list<T>::get_tail() {
	return this->size > 0 ? this->array[this->size-1] : this->defaultValue;
}

template <typename T>
bool fixed_list<T>::is_empty() {
	return this->size == 0;
}

template <typename T>
int fixed_list<T>::get_size() {
	return this->size;
}

template <typename T>
fixed_list_iter<T> fixed_list<T>::cbegin() {
	return fixed_list_iter<T>{*this, 0};
}

template <typename T>
fixed_list_iter<T> fixed_list<T>::cend() {
	return fixed_list_iter<T>{*this, this->size};
}

template <typename T>
fixed_list_iter<T> fixed_list<T>::begin() {
	return this->cbegin();
}

template <typename T>
fixed_list_iter<T> fixed_list<T>::end() {
	return this->cend();
}

template <typename T>
void fixed_list<T>::remove_element(fixed_list_iter<T>& it) {
	if (it.current_cell_index < this->size) {
		for (auto i=(it.current_cell_index); i<this->size; i++) {
			this->array[i] = this->array[i+1];
		}
	}

	this->size--;
}

template <typename T>
void fixed_list<T>::insert_element(fixed_list_iter<T>& it, const T el) {
	this->add_at(it.current_cell_index, el);
}

template <typename T>
T fixed_list<T>::pop_head() {
	T retVal = this->array[0];
	auto it = this->begin();
	this->remove_element(it);
	return retVal;
}

template <typename T>
T& fixed_list<T>::operator[](unsigned int i) {
	return this->array[i];
}

template <typename T>
T fixed_list<T>::operator[](unsigned int i) const {
	return this->array[i];
}

}


#endif /* FIXED_LIST_HPP_ */
