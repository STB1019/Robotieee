/**
 * @file
 *
 * APIs for reading and writing single bits in byte
 *
 * The api has been inspired by <a href="https://stackoverflow.com/a/47990/1887602">this SO answer</a>
 *
 * @date Feb 16, 2018
 * @author koldar
 */

#ifndef BITS_HPP_
#define BITS_HPP_

/**
 * Set to 1 a bit within \c variable
 *
 * @code
 * unsigned char pippo = 0b00000000
 * set_bit(pippo, 3) //0b00001000
 * @endcode
 *
 * @param[in] variable the name of the variable to set. It's a lvalue
 * @param[in] bit_to_set the index (from the right) of the bit to set to 1
 */
#define set_bit(variable, bit_to_set) variable |= 1UL << (bit_to_set)

/**
 * Set to 0 a bit within \c variable
 *
 * @code
 * unsigned char pippo = 0b11111111
 * clear_bit(pippo, 3) //0b11110111
 * @endcode
 *
 * @param[in] variable the name of the variable to clear. It's a lvalue
 * @param[in] bit_to_clear the index (from the right) of the bit to set to 0
 */
#define clear_bit(variable, bit_to_clear) variable &= ~(1UL << (bit_to_clear))

/**
 * Toggle a bit from 0 to 1 or to 1 to 0
 *
 * @code
 * unsigned char pippo = 0b11111111
 * toggle_bit(pippo, 3) //0b11110111
 * toggle_bit(pippo, 3) //0b11111111
 * @endcode
 *
 * @param[in] variable the name of the variable to toggle. It's a lvalue
 * @param[in] bit_to_toggle the index (from the right) of the bit to switch
 */
#define toggle_bit(variable, bit_to_toggle) variable ^= 1UL << (bit_to_toggle)

/**
 * Read the value of a bit
 *
 * @code
 * unsigned char pippo = 0b11111101
 * read_bit(pippo, 3) //value different than 0
 * read_bit(pippo, 1) //value equal to 0
 * @endcode
 *
 * @param[in] bit_to_read the name of the variable to read. It's a lvalue
 * @param[in] bit_to_read the index (from the right) of the bit to read
 * @return 0 if the bit is set to 0, a different value otherwise;
 */
#define read_bit(variable, bit_to_read)  ((variable >> (bit_to_read)) & 1U)

#endif /* BITS_HPP_ */
