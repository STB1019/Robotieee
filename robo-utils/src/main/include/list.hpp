#ifndef CPP_LIST__
#define CPP_LIST__

#include "optional.hpp"

#include <iostream>

namespace robo_utils {

//for iterator definition see https://www.cs.northwestern.edu/~riesbeck/programming/c++/stl-iterator-define.html#TOC4

template<typename T>
struct list_cell;

template <typename T>
class const_list_iter;

template <typename T>
class list_iter;



template<typename T>
class list {
	friend class list_cell<T>;
	friend class const_list_iter<T>;
	friend class list_iter<T>;
private:
	list_cell<T>* head;
	list_cell<T>* tail;
	int size;
	bool destroy_payload;
	T defaultValue;
public:
	void add_to_tail(const T el);
	void add_to_head(const T el);
	T get(int index);
	T get_head();
	T get_tail();
	bool is_empty();
	int get_size();
	const_list_iter<T> cbegin();
	const_list_iter<T> cend();
	list_iter<T> begin();
	list_iter<T> end();
	void remove_element(list_iter<T>& it);
	T pop_head();
	T& operator[](unsigned int i);
	const T& operator[](unsigned int i) const;
public:
	list(T defaultValue, bool destroy_payload);
	~list();
};

template <typename T>
struct list_iter {
	friend class list<T>;
private:
	list<T>& container;
	list_cell<T>* previous_cell;
	list_cell<T>* current_cell;
	list_cell<T>* next_cell;
public:
	list_iter(list<T>& l, list_cell<T>* previous_cell, list_cell<T>* actual_value);
	~list_iter();
public:
	bool operator ==(const list_iter<T>& other);
	bool operator !=(const list_iter<T>& other);
	T& operator*();
	list_iter<T>& operator++();
};

template <typename T>
struct const_list_iter {
	friend class list<T>;
private:
	const list<T>& container;
	const list_cell<T>* current_cell;
public:
	const_list_iter(const list<T>& l, const list_cell<T>* actual_value);
public:
	bool operator ==(const const_list_iter<T>& other);
	bool operator !=(const const_list_iter<T>& other);
	const T& operator*();
	const_list_iter<T>& operator++();
};

/* ******************************* ITERATOR ************************************ */

template <typename T>
list_iter<T>::list_iter(list<T>& l, list_cell<T>* previous_cell, list_cell<T>* actual_value) :
container(l), previous_cell(previous_cell), current_cell(actual_value), next_cell(actual_value != nullptr ? actual_value->next : nullptr) {
}

template <typename T>
list_iter<T>::~list_iter() {
}

template <typename T>
bool list_iter<T>::operator ==(const list_iter<T>& other) {
	if (this == &other) {
		return true;
	}
	return (
			&this->container == &other.container &&
			this->current_cell == other.current_cell &&
			this->next_cell == other.next_cell
	);
}

template <typename T>
bool list_iter<T>::operator !=(const list_iter<T>& other) {
	return !(*this == other);
}

template <typename T>
T& list_iter<T>::operator *() {
	return current_cell->payload;
}

template <typename T>
list_iter<T>& list_iter<T>::operator++() {
	this->previous_cell = this->current_cell;
	this->current_cell = this->next_cell;
	this->next_cell = this->current_cell != nullptr ? this->current_cell->next : nullptr;
	return *this;
}

/* ************************ CONST ITERATOR ****************************** */

template <typename T>
const_list_iter<T>::const_list_iter(const list<T>& l, const list_cell<T>* actual_value) : container(l), current_cell(actual_value) {
}

template <typename T>
bool const_list_iter<T>::operator ==(const const_list_iter<T>& other) {
	if (this == &other) {
		return true;
	}
	return (
			&this->container == &other.container &&
			this->current_cell == other.current_cell
	);
}

template <typename T>
bool const_list_iter<T>::operator !=(const const_list_iter<T>& other) {
	return !(*this == other);
}

template <typename T>
const T& const_list_iter<T>::operator *() {
	return current_cell->payload;
}

template <typename T>
const_list_iter<T>& const_list_iter<T>::operator++() {
	this->current_cell = this->current_cell->next;
	return *this;
}

/* ************************************ LIST **********************************/

template <typename T>
list<T>::list(T defaultValue, bool destroy_payload) : head(nullptr), tail(nullptr), size(0), defaultValue(defaultValue), destroy_payload(destroy_payload) {
}

template <typename T>
list<T>::~list() {
	for(auto it=this->begin(); it != this->end(); ++it) {
		auto payload = *it;
		delete it.current_cell;
		if (this->destroy_payload) {
			delete &payload;
		}
	}
}
template <typename T>
void list<T>::add_to_tail(const T el) {
	if (this->head == nullptr) {
		this->head = new list_cell<T>{el};
		this->tail = this->head;
	} else {
		list_cell<T>* new_tail = new list_cell<T>{el};
		this->tail->next = new_tail;
		this->tail = new_tail;
	}
	this->size++;
}

template <typename T>
void list<T>::add_to_head(const T el) {
	if (this->head == nullptr) {
		this->head = new list_cell<T>{el};
		this->tail = this->head;
	} else {
		list_cell<T>* new_head = new list_cell<T>{el, this->head};
		this->head = new_head;
	}
	this->size++;
}

template <typename T>
T list<T>::get(int index) {
	if (index < 0) {
		return this->defaultValue;
	}
	list_cell<T>* retVal = this->head;
	for (int i=0; i<index; i++) {
		if (retVal == nullptr) {
			return this->defaultValue;
		}
		retVal = retVal->next;
	}
	return (retVal->payload);
}

template <typename T>
T list<T>::get_head() {
	return this->head != nullptr ? this->head->payload : this->defaultValue;
}

template <typename T>
T list<T>::get_tail() {
	return this->tail != nullptr ? this->tail->payload : this->defaultValue;
}

template <typename T>
bool list<T>::is_empty() {
	return this->size == 0;
}

template <typename T>
int list<T>::get_size() {
	return this->size;
}

template <typename T>
void list<T>::remove_element(list_iter<T>& it) {

	if (it.previous_cell == nullptr) {
		//HEAD REMOVAL
		if (it.container.size == 1) {
			//ONLY ELEMENT
			it.container.head = it.next_cell;
			it.container.tail = it.next_cell;

			it.next_cell = nullptr;
		} else {
			it.container.head = it.container.head->next;

			it.next_cell = it.container.head;
		}
	} else if (it.next_cell == nullptr) {
		//TAIL REMOVAL
		//size 1 list scenario is already considered in head removal
		it.container.tail = it.previous_cell;
		it.previous_cell->next = nullptr;

		it.next_cell = nullptr;
	} else {
		//MIDDLE REMOVAL
		it.previous_cell->next = it.next_cell;
	}

	it.container.size--;
	delete it.current_cell;
	it.current_cell = nullptr;
}

template <typename T>
T list<T>::pop_head() {
	if (this->head == nullptr) {
		return nullptr;
	}

	list_cell<T>* old_head = this->head;
	T* retVal = this->head->payload;
	if (this->size == 1) {
		this->head = nullptr;
		this->tail = nullptr;
		delete old_head;
		this->size = 0;
	} else {
		this->head = this->head->next;
		delete old_head;
		this->size--;
	}

	return &(retVal);
}

template <typename T>
T&  list<T>::operator[](unsigned int index) {
	list_cell<T>* retVal = this->head;
	for (int i=0; i<index; i++) {
		retVal = retVal->next;
	}
	return (retVal->payload);
}

template <typename T>
const T&  list<T>::operator[](unsigned int index) const {
	list_cell<T>* retVal = this->head;
	for (int i=0; i<index; i++) {
		retVal = retVal->next;
	}
	return (retVal->payload);
}


template <typename T>
list_iter<T> list<T>::begin() {
	return list_iter<T>{*this, nullptr, this->head};
}

template <typename T>
list_iter<T> list<T>::end() {
	return list_iter<T>{*this, nullptr, nullptr};
}

template <typename T>
const_list_iter<T> list<T>::cbegin() {
	return const_list_iter<T>{*this, this->head};
}

template <typename T>
const_list_iter<T> list<T>::cend() {
	return const_list_iter<T>{*this, nullptr};
}

/* *********************** LIST CELL ***************************** */

template <typename T>
struct list_cell {
public:
	T payload;
	struct list_cell<T>* next;
public:
	list_cell();
	list_cell(T el);
	list_cell(T el, struct list_cell<T>* next);
	~list_cell();
};

template <typename T>
list_cell<T>::list_cell(T el, struct list_cell<T>* next) : payload(el), next(next) {
}

template <typename T>
list_cell<T>::list_cell(T el) : payload(el), next(nullptr) {
}

template <typename T>
list_cell<T>::list_cell() {
	this->payload = nullptr;
	this->next = nullptr;
}

template <typename T>
list_cell<T>::~list_cell() {
}

}

#endif
