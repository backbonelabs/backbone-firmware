/**
 *  @copyright Backbone Labs Inc
 *  @copyright November 24, 2016
 *  @copyright All rights reserved
 *
 *  @author Barry Gackle
 *  @author bdgackle@gmail.com
 */

#ifndef _CYACD_HEADER_RECORD_HPP_
#define _CYACD_HEADER_RECORD_HPP_

// C Standard Headers
#include <cstdint>

// C++ Standard Headers
#include <string>

namespace bb {

/**
 *  Represents the header record of a cyacd file
 *
 *  Parses the first line of a cyacd file and converts the data from ASCII
 *  representation to a format suitable for manipulation.
 */
class HeaderRecord {
 public:
    /** Default constructor */
    HeaderRecord();

    /**
     *  Parse an ASCII line
     *
     *  Parses an ASCII line representing the file header into valid fields.
     *
     *  @param [in] line: The ASCII representation to be parsed.
     *
     *  @throws std::runtime_error if input data cannot be parsed.
     */
    void parse(std::string line);

    /**
     *  Checks validity of checksum in header
     *
     *  Note: since all observed files in the wild have a checksum of zero, this
     *        function currently just verifies that the checksum is zero...
     *
     *  @returns true if the checksum is good, otherwise false.
     */
    bool is_checksum_good() const;

    /** Accessor for Silicon ID field of header record */
    uint32_t silicon_id() const;

    /** Accessor for Silicon Revision field of header */
    uint8_t silicon_rev() const;

    /** Accessor for checksum field of header */
    uint8_t checksum() const;

    /** Generate ASCII string from data in this object */
    std::string ascii() const;

 private:
    uint32_t _silicon_id;
    uint8_t _silicon_rev;
    uint8_t _checksum;

    static constexpr int EXPECTED_BYTES = 6;
};

} // namespace bb

#endif // _CYACD_HEADER_RECORD_HPP_
