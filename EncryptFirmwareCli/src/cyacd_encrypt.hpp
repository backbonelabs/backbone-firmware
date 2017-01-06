/**
 *  @copyright Backbone Labs Inc
 *  @copyright December 11, 2016
 *  @copyright All rights reserved
 *
 *  @author Barry Gackle
 *  @author bdgackle@gmail.com
 */

#ifndef _CYACD_ENCRYPT_HPP_
#define _CYACD_ENCRYPT_HPP_

// Local Headers
#include "cyacd_data_record.hpp"

namespace bb {

/**
 *  Encryption helper function
 *
 *  Given a DataRecord object, this function encrypts the data field with
 *  AES-CBC, a hard coded 128-bit key, and a hard coded initialization vector.
 *  It then outputs a new DataRecord with the encrypted data, and all metadata
 *  fields updated to be valid with the encrypted data.
 *
 *  Printing this back out to a new cyacd file should yield a valid Data Record
 *  that any tools designed for use with cyacd files will accept.
 */
DataRecord encrypt_data_record(const DataRecord& in_rec);

} // namespace bb

#endif // _CYACD_ENCRYPT_HPP_
