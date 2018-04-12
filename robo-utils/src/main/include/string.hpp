/*
 * string.hpp
 *
 *  Created on: Apr 12, 2018
 *      Author: koldar
 */

#ifndef STRING_HPP_
#define STRING_HPP_

namespace robo_utils {

/**
 * Represents a easy wrapper for an underlying buffer
 *
 * \c N represents the number of character allowed to be within
 *
 * @code
 * string<15> s = string<15>{};
 * s.append("hello");
 * s.append(" ");
 * s.append("world");
 * s.getBuffer(); //retrieve the underlying buffer
 * @endcode
 *
 */
template <int N>
class string {
	private:
		/**
		 * the maximum number fo character ::string::buffer can contain
		 */
		const int capacity;
		/**
		 * The index, in ::string::buffer where the first 0 character is located
		 */
		int size;
		/**
		 * The actual buffer itself
		 */
		char buffer[N];
	public:
		/**
		 * get the number of characters within the string
		 *
		 * \note
		 * the terminal character '\0' is not counted
		 *
		 * @param[in] str the string to analyze
		 * @return the number of characters in the string
		 */
		static int _getLengthOfString(const char* str);
		/**
		 * copy the string \c src into \dst by value
		 *
		 * \note
		 * We assume src terminates with a '\0'. \c dst will contain on its tail the '\0' character
		 *
		 * @param[inout] dst the pointer where we start put the data from \c src to;
		 * @param[in] src the pointer where we fetch characters from
		 */
		static void _copyStringInBuffer(char* __restrict__ dst, const char* __restrict__ src);
	public:
		/**
		 * Creates a new string
		 *
		 * @param[in] init the string to hold
		 */
		string(const char* init);
		/**
		 * Creates an empty string
		 */
		string();
		/**
		 * Destroy a string
		 */
		~string();
	public:
		/**
		 * Append some characters to the current string
		 *
		 * @param[in] other the other string
		 * @return
		 * 	\li true if \c other has been fully copied to this
		 * 	\li false if \c this didn't have enough space in its buffer. In this case the string
		 * 		won't be altered **at all**;
		 */
		bool append(const char* other);
		/**
		 * Append some characters to the current string
		 *
		 * @param[in] other the other string
		 * @return
		 * 	\li true if \c other has been fully copied to this
		 * 	\li false if \c this didn't have enough space in its buffer. In this case the string
		 * 		won't be altered **at all**;
		 */
		bool append(const string& s);
		/**
		 * Append a single character to the string
		 *
		 * @param[in] c the character to append
		 * @return
		 * 	\li true if \c other has been fully copied to this
		 * 	\li false if \c this didn't have enough space in its buffer. In this case the string
		 * 		won't be altered **at all**;
		 */
		bool append(char c);
		/**
		 * return the number of characters (null terminator excluded) of this string
		 *
		 * @return the number of characters of the string
		 */
		int getSize() const;
		/**
		 * capacity
		 *
		 * @return the maximum number of characters this string can contain
		 */
		int getCapacity() const;
		/**
		 * @return
		 * 	\li true if the string has no character within it (except for the null terminator);
		 * 	\li false otherwise
		 */
		bool isEmpty() const;
		/**
		 * @return the pointer to the first character of the underlying buffer of the string
		 */
		const char* getBuffer() const;
};

template <int N>
string<N>::string(const char* init) : capacity(N), size(_getLengthOfString(init)) {
	string::_copyStringInBuffer(buffer, init);
}

template <int N>
string<N>::string() : string{""} {

}

template <int N>
string<N>::~string() {
}

template <int N>
bool string<N>::append(const char* other) {
	if (string::_getLengthOfString(other) > (capacity - size)) {
		return false;
	}
	string::_copyStringInBuffer(&buffer[size], other);
	size += string::_getLengthOfString(other);
	return true;
}

template <int N>
bool string<N>::append(const string& s) {
	return append(s.buffer);
}

template <int N>
bool string<N>::append(char c) {
	if (size == capacity) {
		return false;
	}

	buffer[size] = c;
	size++;
	buffer[size] = '\0';

	return true;
}


template <int N>
int string<N>::getSize() const {
	return size;
}

template <int N>
int string<N>::getCapacity() const {
	return capacity;
}

template <int N>
bool string<N>::isEmpty() const {
	return size == 0;
}

template <int N>
const char* string<N>::getBuffer() const {
	return buffer;
}

template <int N>
void string<N>::_copyStringInBuffer(char* __restrict__ dst, const char* __restrict__ src) {
	while(*src!='\0') {
		*dst++ = *src++;
	}
	*dst='\0';
}

template <int N>
int string<N>::_getLengthOfString(const char* str) {
	int retVal = 0;
	while (*str != '\0') {
		retVal++;
		str++;
	}

	return retVal;
}

}


#endif /* STRING_HPP_ */
