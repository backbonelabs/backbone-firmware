/**
 *  @copyright Backbone Labs Inc
 *  @copyright November 24, 2016
 *  @copyright All rights reserved
 *
 *  @author Barry Gackle
 *  @author bdgackle@gmail.com
 *
 *  Various helper functions used throughout the application
 */

#ifndef _CYACD_MISC_HPP_
#define _CYACD_MISC_HPP_

// C Standard Headers
#include <cstdint>

// C++ Standard Headers
#include <string>
#include <vector>

namespace bb {

/**
 *  Convert a numerical digit to its ASCII representation.
 *
 *  @param [in] digit: Numerical reprenstatation of the digit.
 *
 *  @throws std::runtime_error if input digit is not in the ranse 0x00 - 0x0F
 */
char hex_digit_to_ascii(uint8_t digit);

/**
 *  Convert a ASCII digit to its numerical representation.
 *
 *  @param [in] digit: ASCII reprenstatation of the digit.
 *
 *  @throws std::runtime_error if input character is not a valid hex digit,
 *          meaning a numer from 0-9, letter a-f, or letter A-F
 */
uint8_t ascii_digit_to_hex(char digit);

/**
 *  Verify that an ASCII character is a valid hex digit
 *
 *  Determines whether character is considered a hex digit.  For our purposes,
 *  this means it is a numeral from 0-9, letter from a-f, or letter from A-F
 *
 *  @returns true if character is hex digit, false otherwise
 */
bool is_hex_digit(char digit);

/**
 *  Converts a 32-bit integer into a string representation in ASCII, where each
 *  byte is representated by two characters, letters are capital, and the byte
 *  order is big-endian
 */
std::string convert_32_bit_value(uint32_t value);

/**
 *  Converts a 16-bit integer into a string representation in ASCII, where each
 *  byte is representated by two characters, letters are capital, and the byte
 *  order is big-endian
 */
std::string convert_16_bit_value(uint16_t value);

/**
 *  Converts a 8-bit integer into a string representation in ASCII, where each
 *  byte is representated by two characters, letters are capital, and the byte
 *  order is big-endian
 */
std::string convert_8_bit_value(uint8_t value);

/**
 *  Generates a std::vector of numerical bytes from a string containing an
 *  ASCII representation of those bytes.
 */
std::vector<uint8_t> convert_line_to_hex(const std::string& line);

/**
 *
 *  Extract 32-bit field from string
 *
 *  Retrieve numerical representation of a 32-bit field from the ASCII
 *  representation at a given offset.  Assumes big-endian byte ordering.
 *
 *  Note: Inputs are not sanitized.  Giving an invalid index will result
 *  in an exception generated by std::vector.  Including non-hex digits
 *  in the input string will result in a std::runtime_exception being
 *  thrown.
 */
uint32_t extract_32_bit_value(std::vector<uint8_t> line, int index);

/**
 *  Extract 16-bit field from string
 *
 *  Retrieve numerical representation of a 16-bit field from the ASCII
 *  representation at a given offset.  Assumes big-endian byte ordering.
 */
uint16_t extract_16_bit_value(std::vector<uint8_t> line, int index);

/**
 *  Extract 8-bit field from string
 *
 *  Retrieve numerical representation of a 8-bit field from the ASCII
 *  representation at a given offset.  Since we are only extracting one
 *  byte, byte order is not a consideration.
 *
 *  Note: Inputs are not sanitized.  Giving an invalid index will result
 *  in an exception generated by std::vector.  Including non-hex digits
 *  in the input string will result in a std::runtime_exception being
 *  thrown.
 */
uint8_t extract_8_bit_value(std::vector<uint8_t> line, int index);

/**
 *  Calculates the checksum of a group of bytes using the standard Cypress
 *  checksum algorithm of taking two's compliment and subtracting one.
 *
 *  Note: Input is not sanitized.  Passing in a completely empty data vector
 *  will result in an exception being generated by std::vector.
 *
 *  Note: Inputs are not sanitized.  Giving an invalid index will result
 *  in an exception generated by std::vector.  Including non-hex digits
 *  in the input string will result in a std::runtime_exception being
 *  thrown.
 */
uint8_t calculate_checksum(std::vector<uint8_t> data);

/**
 *  XOR two blocks together to yield an output block.  Intended to be
 *  used for block chaining in a AES-CBC scheme, but will work with any two
 *  equally sized blocks of bytes.
 *
 *  Outputs are NOT sanitized.  All three input points must point to valid
 *  buffers of at least AES_BLOCK_SIZE bytes.
 */
void xor_blk(const uint8_t* a, const uint8_t* b, uint8_t* out);

constexpr int AES_BLOCK_SIZE = 16;

} // namespace bb

#endif // _CYACD_MISC_HPP_
