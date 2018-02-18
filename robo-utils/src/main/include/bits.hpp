/*
 * bits.hpp
 *
 *  Created on: Feb 16, 2018
 *      Author: koldar
 */

#ifndef BITS_HPP_
#define BITS_HPP_

//https://stackoverflow.com/a/47990/1887602

#define set_bit(variable, bit_to_set) variable |= 1UL << (bit_to_set)
#define clear_bit(variable, bit_to_clear) variable &= ~(1UL << (bit_to_clear))
#define toggle_bit(variable, bit_to_toggle) variable ^= 1UL << (bit_to_toggle)
#define read_bit(variable, bit_to_read)  ((variable >> (bit_to_read)) & 1U)

#endif /* BITS_HPP_ */
