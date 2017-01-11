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
#include "cyacd_misc.hpp"

// External Headers
#include <cryptopp/aes.h>
#include <cryptopp/ccm.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/filters.h>

// C Standard Headers
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>

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

void encrypt_block(const byte* current, byte* previous, byte* cipher_text)
{
    assert(current != nullptr);
    assert(previous != nullptr);
    assert(cipher_text != nullptr);

    SecByteBlock key(key_bytes, sizeof(key_bytes));

    byte chained_text[AES_BLOCK_SIZE];
    xor_blk(current, previous, chained_text);

    // Crypto++ encryption object
    CCM<AES, TAG_SIZE>::Encryption e;
    e.SetKeyWithIV(key, key.size(), iv, sizeof(iv));
    e.SpecifyDataLengths(0, AES_BLOCK_SIZE, 0);

    // Crypto++ data sink
    ArraySink* sink;
    sink = new ArraySink(cipher_text, AES_BLOCK_SIZE);

    // Crypto++ data filter
    AuthenticatedEncryptionFilter* filter;
    filter = new AuthenticatedEncryptionFilter(e, sink);

    // Crypt++ data source
    ArraySource* source;
    source = new ArraySource(chained_text, AES_BLOCK_SIZE, true, filter);

    delete source;
}

DataRecord encrypt_data_record(const DataRecord& in_rec)
{
    assert(sizeof(key_bytes) == AES::DEFAULT_KEYLENGTH);

    // We would need additional logic for non-multiples of 16;
    // there is no point writing this, since our device uses 256
    // byte flash lines; but we'll check the assumption to be safe
    assert(in_rec.length() % AES_BLOCK_SIZE == 0);

    byte block_buffer[AES_BLOCK_SIZE];
    byte previous_buffer[AES_BLOCK_SIZE];
    DataRecord out_rec;

    memset(block_buffer, 0, AES_BLOCK_SIZE);
    memset(previous_buffer, 0, AES_BLOCK_SIZE);

    int block_count = in_rec.length() / AES_BLOCK_SIZE;
    for (int i = 0; i < block_count; i++) {
        encrypt_block(in_rec.block(i), previous_buffer, block_buffer);
        out_rec.block(block_buffer, i);
        memcpy(previous_buffer, block_buffer, AES_BLOCK_SIZE);
    }

    // Assemble the new data record from encrypted data
    out_rec.array_id(in_rec.array_id());
    out_rec.row_number(in_rec.row_number());
    out_rec.update_checksum();

    return out_rec;
}

} // namespace bb
