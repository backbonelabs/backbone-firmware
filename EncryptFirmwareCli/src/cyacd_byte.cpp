/**
 *  @copyright November 24, 2016
 *  @copyright Backbone Labs Inc
 *  @copyright All rights reserved
 *
 *  @author Barry Gackle
 *  @author bdgackle@gmail.com
 */

// Module Header
#include "cyacd_byte.hpp"
#include "cyacd_misc.hpp"

// C Standard Headers
#include <cstdint>

// C++ Standard Headers
#include <string>

using namespace std;

namespace bb {

CyacdByte::CyacdByte(char upper, char lower) :
    _upper(upper),
    _lower(lower),
    _value(0)
{
    uint8_t hex_upper = ascii_digit_to_hex(upper);
    uint8_t hex_lower = ascii_digit_to_hex(lower);
    _value = hex_upper * 0x10 + hex_lower;
}

CyacdByte::CyacdByte(uint8_t value) :
    _upper('X'),
    _lower('X'),
    _value(value)
{
    uint8_t hex_upper = value / 0x10;
    uint8_t hex_lower = value % 0x10;

    _upper = hex_digit_to_ascii(hex_upper);
    _lower = hex_digit_to_ascii(hex_lower);
}

uint8_t CyacdByte::hex() const
{
    return _value;
}

string CyacdByte::ascii() const
{
    string out;
    out.push_back(_upper);
    out.push_back(_lower);

    return out;
}

} // namespace bb
