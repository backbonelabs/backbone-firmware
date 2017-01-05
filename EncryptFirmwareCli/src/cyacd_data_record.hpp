/**
 *  @copyright Backbone Labs Inc
 *  @copyright November 24, 2016
 *  @copyright All rights reserved
 *
 *  @author Barry Gackle
 *  @author bdgackle@gmail.com
 */

#ifndef _CYACD_DATA_RECORD_HPP_
#define _CYACD_DATA_RECORD_HPP_

// External Headers
#include <cryptopp/cryptlib.h>

// C Standard Headers
#include <cstdint>

// C++ Standard Headers
#include <string>
#include <vector>

namespace bb {

/**
 *  Represents a single data record in a cyacd file
 *
 *  Parses a single ASCII line from a cyacd file, allows for manipulation
 *  of data in that line, and regenerates a new ASCII text line suitable
 *  for printing back to a cyacd file.
 *
 *  Capable of verifying checksum and length fields in the data file.
 */
class DataRecord {
 public:
    /**
     *  Default Constructor
     *
     *  Creates an empty DataRecord.  Data should then be initializes from an
     *  actual cyacd file, or else manually set and used to generate a line of
     *  a synthetic cyacd file.
     */
    DataRecord();

    /** Load data into this object from a line of a cyacd file */
    void parse(std::string line);

    /** Verify checksum of current line */
    bool is_checksum_good() const;

    /** Verify length field of current line */
    bool is_length_good() const;

    /** Compute expected checksum for data in current line */
    uint8_t expected_checksum() const;

    /** Accessor for Array ID field of Data Record header */
    uint8_t array_id() const;

    /** Accesor for Row Number field of Data Record header */
    uint16_t row_number() const;

    /** Accessor for length field of Data Record header */
    uint16_t length() const;

    /** Accessor for data field of Data Record */
    const uint8_t* data() const;

    /** Access a single block of the data field */
    const byte* block(int block_num) const;

    /** Accessor for checksum field of Data Record */
    uint8_t checksum() const;

    /** Mutator for Array ID field of header */
    void array_id(uint8_t id);

    /** Mutator for Row Number field of header */
    void row_number(uint16_t num);

    /** Mutator for data field fo Data Record */
    void data(const uint8_t* buf, int len);

    /** Mutator for a given block of data */
    void block(const byte* buf, int block_num);

    /** Generate string suitable for printing in cyacd file */
    std::string ascii() const;

    /** Number of bytes in a data record header */
    static constexpr int HEADER_BYTES = 5;

    /** Number of ASCII characters used to encode a record header */
    static constexpr int HEADER_CHARS = HEADER_BYTES * 2;

 private:
    uint8_t _array_id;
    uint16_t _row_number;
    uint16_t _length;
    uint8_t _checksum;
    std::vector<uint8_t> _data;
};

} // namespace bb

#endif // _CYACD_DATA_RECORD_HPP_
