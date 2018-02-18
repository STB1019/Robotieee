/**
 * @file
 *
 * API providing a wrapper for potentially null values
 *
 * @date Feb 14, 2018
 * @author koldar
 */

#ifndef OPTIONAL_HPP_
#define OPTIONAL_HPP_

namespace robo_utils {

/**
 *Represents a structure containing a value that might be null
 *
 * @code
 * optional<int> opt = optional<int>::of(); //create null int
 * optional<int> opt2 = optional<int>::of(4); //creeate int which is not null
 *
 * int v = opt.get_or_else(0) //this will set v to 0
 * int w = opt2.get_or_else(0) //this will set tv to 4
 * @endcode
 */
template <typename T>
struct optional {
private:
	/**
	 * The value wrapped.
	 *
	 * If optional::present is \c false, the bits within the field are random
	 */
	T value;
	/**
	 * \c true if optional::value field has some meaning; \c false otherwise
	 */
	bool present;
	/**
	 * Initialize an structur with meaningless optional::value
	 */
	optional<T>();
	/**
	 * Initialize an structure with meaningful optional::value
	 */
	optional<T>(const T& value);
public:
	/**
	 * Check if the structure actually wraps some value
	 *
	 * @return
	 * 	\li \c true if the value wraps some value;
	 * 	\li \c false otherwise;
	 */
	bool is_present();
	/**
	 * Unwrap a value from the optional
	 *
	 * @param[in] d a value to return if the structure doesn't wrap a meaningful value
	 * @return a value unwrapper from the structure
	 */
	T get_or_else(const T& d);
public:
	/**
	 * creates a structure which does not wrap anything at all
	 */
static optional<T> of();
	/**
	 * Create a structure which wrap a non null value \c value
	 *
	 * @param[in] value the value to wrap
	 */
static optional<T> of(const T& value);
};

template <typename T>
optional<T>::optional() {
	this->present = false;
}

template <typename T>
optional<T>::optional(const T& value) {
	this->present = true;
	this->value = value;
}

template <typename T>
bool optional<T>::is_present() {
	return this->present;
}

template <typename T>
T optional<T>::get_or_else(const T& d) {
	return this->is_present() ? this->value : d;
}

template <typename T>
optional<T> of(const T& value) {
	return optional<T>{value};
}

template <typename T>
optional<T> of() {
	return optional<T>{};
}

}

#endif /* OPTIONAL_HPP_ */
