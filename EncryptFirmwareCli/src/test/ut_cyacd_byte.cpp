/**
 *  Copyright (c) 2016, Backbone Labs Inc
 */

// Local Headers
#include "cyacd_byte.hpp"

// External Headers
#include <check.h>

// C++ Standard Headers
#include <stdexcept>

using namespace bb;
using namespace std;

static const char _hex_digits[16] = { '0', '1', '2', '3', '4',
                                     '5', '6', '7', '8', '9',
                                     'A', 'B', 'C', 'D', 'E', 'F' };

START_TEST(test_hex_to_ascii)
{
    for (uint8_t i = 0; i <= 0xF; i++) {
        for (uint8_t j = 0; j <= 0xF; j++) {
            uint8_t input = i * 0x10 + j;

            CyacdByte expected(_hex_digits[i], _hex_digits[j]);
            CyacdByte actual(input);

            ck_assert(expected.hex() == actual.hex());
            ck_assert(expected.ascii() == actual.ascii());
        }
    }
}
END_TEST

START_TEST(test_ascii_to_hex)
{
    for (int i = 0; i < 0x10; i++) {
        for (int j = 0; j < 0x10; j++) {
            CyacdByte expected(i * 0x10 + j);
            CyacdByte actual(_hex_digits[i], _hex_digits[j]);

            ck_assert(expected.hex() == actual.hex());
            ck_assert(expected.ascii() == actual.ascii());
        }
    }
}
END_TEST

START_TEST(test_ascii_to_hex_bad_upper)
{
    bool exception_caught = false;

    try {
        CyacdByte('G', '0');
    } catch (exception& e) {
        exception_caught = true;
    }

    ck_assert(exception_caught);
}
END_TEST

START_TEST(test_ascii_to_hex_bad_lower)
{
    bool exception_caught = false;

    try {
        CyacdByte('0', 'G');
    } catch (exception& e) {
        exception_caught = true;
    }

    ck_assert(exception_caught);
}
END_TEST

int main(void)
{
    TCase *tc = tcase_create("ascii");
    tcase_add_test(tc, test_hex_to_ascii);
    tcase_add_test(tc, test_ascii_to_hex);
    tcase_add_test(tc, test_ascii_to_hex_bad_upper);
    tcase_add_test(tc, test_ascii_to_hex_bad_lower);

    Suite* s = suite_create("Byte conversion class");
    suite_add_tcase(s, tc);

    SRunner* sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    int number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return number_failed;
}
