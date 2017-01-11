/**
 *  @copyright November 24, 2016
 *  @copyright Backbone Labs Inc
 *  @copyright All rights reserved
 *
 *  @author Barry Gackle
 *  @author bdgackle@gmail.com
 */

// Module Header
#include "cyacd_data_record.hpp"

// Local Headers
#include "cyacd_misc.hpp"

// External Headers
#include <cryptopp/cryptlib.h>

// C Standard Headers
#include <cstddef>
#include <cstdint>

// C++ Standard Headers
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

namespace bb {

DataRecord::DataRecord() :
    _array_id(0),
    _row_number(0),
    _length(0),
    _checksum(0),
    _data() {}

void DataRecord::parse(string line)
{
    // Make sure we have a full header
    if (line.size() < HEADER_CHARS) {
        throw runtime_error("Got short data line");
    }

    vector<uint8_t> all_bytes = convert_line_to_hex(line);

    _array_id = extract_8_bit_value(all_bytes, 0);
    _row_number = extract_16_bit_value(all_bytes, 1);
    _length = extract_16_bit_value(all_bytes, 3);
    _checksum = extract_8_bit_value(all_bytes, all_bytes.size() - 1);

    _data.clear();
    vector<uint8_t>::const_iterator first = all_bytes.begin() + HEADER_BYTES;
    vector<uint8_t>::const_iterator last = all_bytes.begin() + all_bytes.size() - 1;
    vector<uint8_t> data(first, last);
    _data = data;

    // TODO: Verify length and data length match
    if (!is_checksum_good())
    {
        throw runtime_error("invalid checksum");
    }
}

void DataRecord::update_checksum()
{
    _checksum = expected_checksum();
}

bool DataRecord::is_checksum_good() const
{
    return _checksum == expected_checksum();
}

bool DataRecord::is_length_good() const
{
    return _length == _data.size();
}

uint8_t DataRecord::expected_checksum() const
{
    vector<uint8_t> data_and_header;

    data_and_header.push_back(_array_id);
    data_and_header.push_back(_row_number / 0x100);
    data_and_header.push_back(_row_number % 0x100);
    data_and_header.push_back(_length / 0x100);
    data_and_header.push_back(_length % 0x100);

    // Yes, this wastes a few cycles.  Oh well.  It's a PC.  That's its job.
    for (size_t i = 0; i < _data.size(); i++) {
        data_and_header.push_back(_data.at(i));
    }

    return calculate_checksum(data_and_header);
}

uint8_t DataRecord::array_id() const
{
    return _array_id;
}

void DataRecord::array_id(uint8_t id)
{
    _array_id = id;
}

uint16_t DataRecord::row_number() const
{
    return _row_number;
}

void DataRecord::row_number(uint16_t num)
{
    _row_number = num;
}

uint16_t DataRecord::length() const
{
    return _length;
}

uint8_t DataRecord::checksum() const
{
    return _checksum;
}

const uint8_t* DataRecord::data() const
{
    return _data.data();
}

const byte* DataRecord::block(int block_num) const
{
    return _data.data() + block_num * AES_BLOCK_SIZE;
}

void DataRecord::data(const uint8_t* buf, int len)
{
    _data.clear();

    for (int i = 0; i < len; i++) {
        _data.push_back(buf[i]);
    }

    _length = len;
    _checksum = expected_checksum();
}

void DataRecord::block(const byte* buf, int block_num)
{
    size_t min_size = block_num * AES_BLOCK_SIZE + AES_BLOCK_SIZE;

    if (_data.size() < min_size) {
        for (size_t i = _data.size(); i < min_size; i++) {
            _data.push_back(0);
            _length++;
        }
    }
    int base = block_num * AES_BLOCK_SIZE;
    for (int i = 0; i < AES_BLOCK_SIZE; i++) {
        _data.at(base + i) = buf[i];
    }
}

string DataRecord::ascii() const
{
    string out;

    out += convert_8_bit_value(_array_id);
    out += convert_16_bit_value(_row_number);
    out += convert_16_bit_value(_length);

    for (int i = 0; i < (int)_data.size(); i++) {
        out += convert_8_bit_value(_data.at(i));
    }

    out += convert_8_bit_value(_checksum);

    return out;
}

} // namespace bb
