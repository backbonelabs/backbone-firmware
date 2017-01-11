/**
 *  Copyright (c) 2016, Backbone Labs Inc
 */

// Module Header
#include "cyacd_misc.hpp"

// Local Headers
#include "cyacd_byte.hpp"

// External Headers
#include <check.h>

// C Standard Headers
#include <cstddef>
#include <cstdint>

// C++ Standard Headers
#include <string>
#include <vector>

using namespace bb;
using namespace std;

static const char _hex_digits[16] = { '0', '1', '2', '3', '4',
                                     '5', '6', '7', '8', '9',
                                     'A', 'B', 'C', 'D', 'E', 'F' };

START_TEST(test_is_hex_digit)
{
    for (uint8_t i = 0; i < 0xFF; i++) {
        if ((i >= '0') && (i <= '9')) {
            ck_assert(is_hex_digit(i));
        }
        else if ((i >= 'a') && (i <= 'f')) {
            ck_assert(is_hex_digit(i));
        }
        else if ((i >= 'A') && (i <= 'F')) {
            ck_assert(is_hex_digit(i));
        }
        else {
            ck_assert(!is_hex_digit(i));
        }
    }

    ck_assert(!is_hex_digit((char)0xFF));
}
END_TEST

START_TEST(test_convert_line_to_hex)
{
    string line = "135111A30000";
    vector<uint8_t> expected = { 0x13, 0x51, 0x11, 0xA3, 0x00, 0x00 };
    vector<uint8_t> actual = convert_line_to_hex(line);

    ck_assert(actual.size() == expected.size());
    for (size_t i = 0; i < expected.size(); i++) {
        ck_assert(actual.at(i) == expected.at(i));
    }
}
END_TEST

START_TEST(test_extract_32_bit_value)
{
    string line = "135111A30000";
    vector<uint8_t> in = { 0x13, 0x51, 0x11, 0xA3, 0x00, 0x00 };

    ck_assert(extract_32_bit_value(in, 0) == 0x135111A3);
    ck_assert(extract_32_bit_value(in, 1) == 0x5111A300);
    ck_assert(extract_32_bit_value(in, 2) == 0x11A30000);
}
END_TEST

START_TEST(test_extract_16_bit_value)
{
    string line = "135111A30000";
    vector<uint8_t> in = { 0x13, 0x51, 0x11, 0xA3, 0x00, 0x00 };

    ck_assert(extract_16_bit_value(in, 0) == 0x1351);
    ck_assert(extract_16_bit_value(in, 1) == 0x5111);
    ck_assert(extract_16_bit_value(in, 2) == 0x11A3);
}
END_TEST

START_TEST(test_extract_8_bit_value)
{
    string line = "135111A30000";
    vector<uint8_t> in = { 0x13, 0x51, 0x11, 0xA3, 0x00, 0x00 };

    ck_assert(extract_8_bit_value(in, 0) == 0x13);
    ck_assert(extract_8_bit_value(in, 1) == 0x51);
    ck_assert(extract_8_bit_value(in, 2) == 0x11);
}
END_TEST

START_TEST(test_calculate_checksum)
{
    vector<uint8_t> in1 = { 0, 0, 0, 0, 0, 0, 0, 0 };
    uint8_t expected1 = 0;

    vector<uint8_t> in2 = { 1, 2, 3, 4, 5, 6, 7, 8 };
    uint8_t expected2 = 0xdc;

    uint8_t actual1 = calculate_checksum(in1);
    ck_assert(actual1 == expected1);

    uint8_t actual2 = calculate_checksum(in2);
    ck_assert(actual2 == expected2);
}
END_TEST

int main(void)
{
    TCase *tc = tcase_create("ascii");
    tcase_add_test(tc, test_is_hex_digit);
    tcase_add_test(tc, test_convert_line_to_hex);
    tcase_add_test(tc, test_extract_32_bit_value);
    tcase_add_test(tc, test_extract_16_bit_value);
    tcase_add_test(tc, test_extract_8_bit_value);
    tcase_add_test(tc, test_calculate_checksum);

    Suite* s = suite_create("Misc helper functions");
    suite_add_tcase(s, tc);

    SRunner* sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    int number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return number_failed;
}
