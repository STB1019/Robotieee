/*
 * string.hpp
 *
 *  Created on: Apr 12, 2018
 *      Author: koldar
 */

#ifndef STRING_HPP_
#define STRING_HPP_

namespace robo_utils {

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
		char buffer[N];
	public:
		static int _getLengthOfString(const char* str);
		static void _copyStringInBuffer(char* __restrict__ dst, const char* __restrict__ src);
	public:
		string(const char* init);
		string();
		~string();
	public:
		bool append(const char* other);
		bool append(const string& s);
		bool append(char c);
		int getSize() const;
		int getCapacity() const;
		bool isEmpty() const;
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
