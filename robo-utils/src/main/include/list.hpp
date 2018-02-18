/**
 * @file
 *
 * Provide a small forward list implementation
 *
 * @author koldar
 * @date 18 Feb 2018
 */

#ifndef LIST_HPP_
#define LIST_HPP_

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
	/**
	 * the first element of the list
	 */
	list_cell<T>* head;
	/**
	 * The last element of the list
	 */
	list_cell<T>* tail;
	/**
	 * the number of elements within the list
	 */
	int size;
	/**
	 * true if the destructor method should remove from memory the items \c T within the list.
	 *
	 * If they are not pointers it should be \c false
	 */
	bool destroy_payload;
	/**
	 * A value to return if an operation of the list fails
	 */
	T defaultValue;
public:
	/**
	 * Adds an element at the end of the list
	 *
	 * @param[in] el the element to add to the tail of the list
	 */
	void add_to_tail(const T el);
	/**
	 * Adds an element at the beginning of the list
	 *
	 * @param[in] el the element to add to the ehad of the list
	 */
	void add_to_head(const T el);
	/**
	 * Get the i-th element of the list
	 *
	 * @param[in] index the index of the element to fetch
	 * @return
	 * 	\li the element in the list;
	 * 	\li the default value if \c index leads to no item
	 */
	T get(int index);
	/**
	 * Get the first element of the list
	 *
	 * @return
	 * 	\li the head of the list;
	 * 	\li the default value if the list is empty
	 */
	T get_head();
	/**
	 * Get the last element of the lsit
	 *
	 * @return
	 * 	\li the last element of the list;
	 * 	\li the default value if the list is empty
	 */
	T get_tail();
	/**
	 * Check if the list is empty
	 *
	 * @return \c true if the list is empty; \c false otherwise
	 */
	bool is_empty();
	/**
	 * The number of elements
	 *
	 * @return the number of elements within the list
	 */
	int get_size();
	/**
	 * method to implement a constant iteration on the list
	 *
	 * @includedoc list_iterator_example.doxy
	 *
	 * @return a constant iterator pointing to the first item of the list
	 */
	const_list_iter<T> cbegin();
	/**
	 * method to implement a constant iteration on the list
	 *
	 * @includedoc list_iterator_example.doxy
	 *
	 * @return a constant iterator pointing to the last item of the list
	 */
	const_list_iter<T> cend();
	/**
	 * method to implement an iteration on the list
	 *
	 * @includedoc list_iterator_example.doxy
	 *
	 * @return an iterator pointing to the first item of the list
	 */
	list_iter<T> begin();
	/**
	 * method to implement an iteration on the list
	 *
	 * @includedoc list_iterator_example.doxy
	 *
	 * @return an iterator pointing to the last item of the list
	 */
	list_iter<T> end();
	/**
	 * Remove an item of the list during iteration of the list
	 *
	 * @includedoc list_iterator_example.doxy
	 *
	 * \post
	 * 	\li this will remove the element from the list
	 *
	 * @param[in] it the point where the iterator is currently
	 */
	void remove_element(list_iter<T>& it);
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
	T pop_head();
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
	T& operator[](unsigned int i);
	/**
	 * A constant lvalue to the i-th element of the list
	 *
	 * @param[in] i the index of the cell to update
	 * @return the lvalue representing the cell to (possibly) change
	 */
	const T& operator[](unsigned int i) const;
public:
	/**
	 * Initialize the list
	 *
	 * @param[in] defaultValue the value to return in case some operation couldn't be performed for some reasons
	 * @param[in] destroy_payload if true, \c delete will delete the items within the list as well. Useful only if the payloads are actually pointers.
	 */
	list(T defaultValue, bool destroy_payload);
	/**
	 * Dealloc the list
	 */
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
