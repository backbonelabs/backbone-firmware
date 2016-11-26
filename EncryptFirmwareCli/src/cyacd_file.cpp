/**
 *  @copyright November 24, 2016
 *  @copyright Backbone Labs Inc
 *  @copyright All rights reserved
 *
 *  @author Barry Gackle
 *  @author bdgackle@gmail.com
 */

// Module Header
#include "cyacd_file.hpp"

// Local Headers
#include "cyacd_data_record.hpp"
#include "cyacd_encrypt.hpp"
#include "cyacd_header_record.hpp"

// C Standard Headers
#include <cstddef>

// C++ Standard Headers
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

namespace bb {

void CyacdFile::load(string name)
{
    ifstream file(name);

    if (!file.is_open()) {
        throw runtime_error("Could not open input file.");
    }

    string line;
    getline(file, line);
    _header_record.parse(line);

    _data_records.clear();
    string data_line;
    while (getline(file, data_line)) {
        DataRecord record;
        record.parse(data_line.substr(1));
        _data_records.push_back(record);
    }

    file.close();
}

void CyacdFile::encrypt()
{
    // Cache the clear text and clear data records
    vector<DataRecord> clear_text = _data_records;
    size_t count = data_record_count();
    _data_records.clear();

    for (size_t i = 0; i < count; i++) {
        DataRecord clear = clear_text.at(i);
        DataRecord cipher = encrypt_data_record(clear);
        _data_records.push_back(cipher);
    }
}

void CyacdFile::save(string name)
{
    ofstream file(name, ios::out);

    if (!file.is_open()) {
        throw runtime_error("Could not open output file.");
    }

    file << _header_record.ascii() << "\r\n";

    for (size_t i = 0; i < _data_records.size(); i++) {
        file << ":" << _data_records.at(i).ascii() << "\r\n";
    }

    file.close();
}

HeaderRecord CyacdFile::header() const
{
    return _header_record;
}

DataRecord CyacdFile::data(int index) const
{
    return _data_records.at(index);
}

size_t CyacdFile::data_record_count() const
{
    return _data_records.size();
}

} // namespace bb
