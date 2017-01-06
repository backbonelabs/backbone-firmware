/**
 *  @copyright November 24, 2016
 *  @copyright Backbone Labs Inc
 *  @copyright All rights reserved
 *
 *  @author Barry Gackle
 *  @author bdgackle@gmail.com
 */

// Module Header
#include "cyacd_misc.hpp"

// Local Headers
#include "cyacd_byte.hpp"

// External Headers
#include <cryptopp/cryptlib.h>

// C Standard Headers
#include <cstddef>
#include <cstdint>

// C++ Standard Headers
#include <string>
#include <vector>
#include <stdexcept>

using namespace std;

namespace bb {

char hex_digit_to_ascii(uint8_t digit)
{
    switch (digit) {
        case 0:     return '0';
        case 1:     return '1';
        case 2:     return '2';
        case 3:     return '3';
        case 4:     return '4';
        case 5:     return '5';
        case 6:     return '6';
        case 7:     return '7';
        case 8:     return '8';
        case 9:     return '9';
        case 0x0A:  return 'A';
        case 0x0B:  return 'B';
        case 0x0C:  return 'C';
        case 0x0D:  return 'D';
        case 0x0E:  return 'E';
        case 0x0F:  return 'F';
        default:    throw runtime_error("Bad hex digit\n");
    }
}

uint8_t ascii_digit_to_hex(char digit)
{
    switch (digit) {
        case '0':           return 0;
        case '1':           return 1;
        case '2':           return 2;
        case '3':           return 3;
        case '4':           return 4;
        case '5':           return 5;
        case '6':           return 6;
        case '7':           return 7;
        case '8':           return 8;
        case '9':           return 9;
        case 'a': case 'A': return 0xa;
        case 'b': case 'B': return 0xb;
        case 'c': case 'C': return 0xc;
        case 'd': case 'D': return 0xd;
        case 'e': case 'E': return 0xe;
        case 'f': case 'F': return 0xf;
        default :           throw runtime_error("Bad hex digit\n");
    }
}

bool is_hex_digit(char digit)
{
    switch(digit) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        case 'a': case 'A': case 'b': case 'B':
        case 'c': case 'C': case 'd': case 'D':
        case 'e': case 'E': case 'f': case 'F':
            return true;

        default:
            return false;
    }
}

vector<uint8_t> convert_line_to_hex(const string& line)
{
    vector<uint8_t> out;

    for (size_t i = 0; i < line.length() / 2; i++) {
        CyacdByte byte(line.at(i * 2), line.at(i * 2 + 1));
        out.push_back(byte.hex());
    }

    return out;
}

string convert_32_bit_value(uint32_t value)
{
    CyacdByte b0((value & 0xFF000000) >> 24);
    CyacdByte b1((value & 0x00FF0000) >> 16);
    CyacdByte b2((value & 0x0000FF00) >>  8);
    CyacdByte b3((value & 0x000000FF) >>  0);

    string out = b0.ascii() + b1.ascii() + b2.ascii() + b3.ascii();
    return out;
}

string convert_16_bit_value(uint16_t value)
{
    CyacdByte b0((value & 0xFF00) >> 8);
    CyacdByte b1((value & 0x00FF) >> 0);

    string out = b0.ascii() + b1.ascii();
    return out;
}

string convert_8_bit_value(uint8_t value)
{
    CyacdByte b(value);

    return b.ascii();
}

uint32_t extract_32_bit_value(vector<uint8_t> line, int index)
{
    return (line.at(index + 0) << 24) +
           (line.at(index + 1) << 16) +
           (line.at(index + 2) <<  8) +
           (line.at(index + 3) <<  0);
}

uint16_t extract_16_bit_value(vector<uint8_t> line, int index)
{
    return (line.at(index + 0) << 8) +
           (line.at(index + 1) << 0);
}

uint8_t extract_8_bit_value(vector<uint8_t> line, int index)
{
    return (line.at(index));
}

uint8_t calculate_checksum(vector<uint8_t> data)
{
    uint8_t sum;
    for (size_t i = 0; i < data.size(); i++) {
        sum += data.at(i);
    }

    return ~sum + 1;
}

void xor_blk(const byte* a, const byte* b, byte* out)
{
    for (int i = 0; i < AES_BLOCK_SIZE; i++) {
        out[i] = a[i] ^ b[i];
    }
}

} // namespace bb
