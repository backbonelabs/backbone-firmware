/**
 *  @copyright Backbone Labs Inc
 *  @copyright November 24, 2016
 *  @copyright All rights reserved
 *
 *  @author Barry Gackle
 *  @author bdgackle@gmail.com
 *
 *  Functions for converting the ASCII hex representation used in cyacd files
 *  into logical bytes and vice-versa.
 */

#ifndef _CYACD_BYTE_HPP
#define _CYACD_BYTE_HPP

// C Standard Headers
#include <cstdint>

// C++ Standard Headers
#include <string>

namespace bb {

/**
 *  Contains a single byte from a cyacd file
 *
 *  Converts back and forth between logical representation of the byte, and
 *  ASCII representation as used in cyacd files.
 */
class CyacdByte {
 public:
    /** Construct from two ASCII bytes, as found in a cyacd file. */
    CyacdByte(char upper, char lower);

    /** Construct directly from a numerical byte. */
    CyacdByte(uint8_t value);

    /** Default constructor; provided for compatibility with C++ containers */
    CyacdByte() = default;

    /** Accessor for numerical representation of this byte. */
    uint8_t hex() const;

    /** Accessor for ASCII representation of this byte. */
    std::string ascii() const;

 private:
    char _upper;
    char _lower;
    uint8_t _value;
};

} // namespace bb

#endif // _CYACD_BYTE_HPP
