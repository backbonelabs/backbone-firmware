/**
 *  @copyright Backbone Labs Inc
 *  @copyright December 11, 2016
 *  @copyright All rights reserved
 *
 *  @author Barry Gackle
 *  @author bdgackle@gmail.com
 */

// Local Headers
#include "cyacd_data_record.hpp"

// External Headers
#include <cryptopp/aes.h>
#include <cryptopp/ccm.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/filters.h>

// C Standard Headers
#include <cassert>
#include <cstddef>
#include <cstdint>

// C++ Standard Headers
#include <vector>

using namespace std;
using namespace CryptoPP;

namespace bb {
const int TAG_SIZE = 4;

// TODO: Replace with something known but random
byte key_bytes[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF,
                     0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF };

// TODO: Replace with something known but random
byte iv[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
              0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c };

DataRecord encrypt_data_record(const DataRecord& in_rec)
{
    assert(sizeof(key_bytes) == AES::DEFAULT_KEYLENGTH);

    SecByteBlock key(key_bytes, sizeof(key_bytes));

    // Allocate memory for encryption/decryption
    size_t data_size = in_rec.length();
    byte* clear_text = (byte *)in_rec.data();
    byte* cipher_text = new byte[data_size];

    // Crypto++ encryption object
    CCM<AES, TAG_SIZE>::Encryption e;
    e.SetKeyWithIV(key, key.size(), iv, sizeof(iv));
    e.SpecifyDataLengths(0, data_size, 0);

    // Crypto++ data sink
    ArraySink* sink;
    sink = new ArraySink(cipher_text, data_size);

    // Crypto++ data filter
    AuthenticatedEncryptionFilter* filter;
    filter = new AuthenticatedEncryptionFilter(e, sink);

    // Crypt++ data source
    ArraySource* source;
    source = new ArraySource(clear_text, data_size, true, filter);
    (void)source; // Squelch a compiler warning

    // Assemble the new data record from encrypted data
    DataRecord out_rec;
    out_rec.array_id(in_rec.array_id());
    out_rec.row_number(in_rec.row_number());
    out_rec.data(cipher_text, data_size);

    return out_rec;
}

} // namespace bb
