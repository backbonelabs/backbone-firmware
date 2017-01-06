/**
 *  @copyright Backbone Labs Inc
 *  @copyright November 26, 2016
 *  @copyright All rights reserved
 *
 *  @author Barry Gackle
 *  @author bdgackle@gmail.com
 */

// Module Header
#include "cyacd_header_record.hpp"

// Local Headers
#include "cyacd_misc.hpp"

// C Standard Headers
#include <cstdint>

// C++ Standard Headers
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

namespace bb {

HeaderRecord::HeaderRecord() :
    _silicon_id(0),
    _silicon_rev(0),
    _checksum(0) {}

void HeaderRecord::parse(string line)
{
    vector<uint8_t> header_bytes = convert_line_to_hex(line);

    if (header_bytes.size() != EXPECTED_BYTES) {
        throw std::runtime_error("Header record wrong size");
    }

    _silicon_id = extract_32_bit_value(header_bytes, 0);
    _silicon_rev = extract_8_bit_value(header_bytes, 4);
    _checksum = extract_8_bit_value(header_bytes, 5);

    if (!is_checksum_good()) {
        throw std::runtime_error("Header record bad checksum");
    }
}

bool HeaderRecord::is_checksum_good() const
{
    // I am seeing cyacd files in the wild that don't have checksums
    // on the header line.  Until we resolve this, this test needs to
    // always just pass.
    // The only cyacd files I see in the wild have '0' as the header
    // checksum, contrary to the documentation
    if (_checksum == 0) {
        return true;
    }

    return false;
}

uint32_t HeaderRecord::silicon_id() const
{
    return _silicon_id;
}

uint8_t HeaderRecord::silicon_rev() const
{
    return _silicon_rev;
}

uint8_t HeaderRecord::checksum() const
{
    return _checksum;
}

string HeaderRecord::ascii() const
{
    string id_str = convert_32_bit_value(_silicon_id);
    string rev_str = convert_8_bit_value(_silicon_rev);
    string checksum_str = convert_8_bit_value(_checksum);

    string out = id_str + rev_str + checksum_str;

    return out;
}

} // namespace bb
