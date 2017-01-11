/**
 *  @copyright Backbone Labs Inc
 *  @copyright November 24, 2016
 *  @copyright All rights reserved
 *
 *  @author Barry Gackle
 *  @author bdgackle@gmail.com
 */

#ifndef _CYACD_FILE_HPP_
#define _CYACD_FILE_HPP_

// Local Headers
#include "cyacd_data_record.hpp"
#include "cyacd_header_record.hpp"

// C Standard Headers
#include <cstddef>

// C++ Standard Headers
#include <string>
#include <vector>

namespace bb {

/**
 *  Represents an entire cyacd file.
 *
 *  Parses an entire cyacd file into a HeaderRecord object and a series of
 *  DataRecord objects.
 *
 *  Capabile of encrypting the data fields of this file and outputting an
 *  equivalent encrypted version of the parsed file.
 *
 *  To use, use the @ref load() function to parse a file, call @ref encrypt() to
 *  encrypt the file, and then output the new encrypted cyacd file using the
 *  @ref save() function.
 */
class CyacdFile {
 public:
    /** Default constructor */
    CyacdFile() = default;

    /**
     *  Load and parse a cyacd file.
     *
     *  Loads a cyacd file from the given path, and parses that into a logical
     *  representation of the data in the file.
     *
     *  @param [in] name: Path to file to parse.
     *
     *  @throws std::runtime_error if file could not be opened.
     */
    void load(std::string name);

    /**
     *  Encrypt the data
     *
     *  Transforms the data in the object into an equivalent encrypted
     *  representation using AES-CBC with a hard coded 128-bit key and
     *  hard coded 12-byte initialization vector.
     */
    void encrypt();

    /**
     *  Output data currently stored in this object as a cyacd file.
     *
     *  Creates an ascii representation of the data in this file and outputs
     *  it as a standard
     */
    void save(std::string name);

    /** Accessor for the header record of this file */
    HeaderRecord header() const;

    /** Accessor for data records of this file */
    DataRecord data(int index) const;

    /** Accessor for data record count in this file */
    size_t data_record_count() const;

 private:
    HeaderRecord _header_record;
    std::vector<DataRecord> _data_records;
};

} // namespace bb

#endif // _CYACD_FILE_HPP_
