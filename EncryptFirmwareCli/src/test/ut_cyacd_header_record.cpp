/**
 *  Copyright (c) 2016, Backbone Labs Inc
 */

// Local Headers
#include "cyacd_header_record.hpp"

// External Headers
#include <check.h>

// C++ Standard Headers
#include <string>

using namespace bb;
using namespace std;

START_TEST(test_constructor)
{
    HeaderRecord record;

    ck_assert(record.silicon_id() == 0);
    ck_assert(record.silicon_rev() == 0);
    ck_assert(record.checksum() == 0);
}
END_TEST

START_TEST(test_parse)
{
    const string line = "135111A3DE00";
    HeaderRecord record;
    record.parse(line);

    ck_assert(record.silicon_id() == 0x135111A3);
    ck_assert(record.silicon_rev() == 0xDE);
    ck_assert(record.checksum() == 0x00);
    ck_assert(record.is_checksum_good());
}
END_TEST

START_TEST(test_parse_bad_length_short)
{
    bool thrown = false;
    const string line = "135111A3DE";
    HeaderRecord record;

    try {
        record.parse(line);
    } catch (std::exception& e) {
        thrown = true;
    }

    ck_assert(thrown == true);
}
END_TEST

START_TEST(test_parse_bad_length_long)
{
    bool thrown = false;
    const string line = "135111A3DE0000";
    HeaderRecord record;

    try {
        record.parse(line);
    } catch (std::exception& e) {
        thrown = true;
    }

    ck_assert(thrown == true);
}
END_TEST

START_TEST(test_parse_bad_checksum)
{
    bool thrown = false;
    const string line = "135111A3DE01";
    HeaderRecord record;

    try {
        record.parse(line);
    } catch (std::exception& e) {
        thrown = true;
    }

    ck_assert(thrown == true);
}
END_TEST

START_TEST(test_parse_bad_digit)
{
    bool thrown = false;
    const string line = "135111A3DE0R";
    HeaderRecord record;

    try {
        record.parse(line);
    } catch (std::exception& e) {
        thrown = true;
    }

    ck_assert(thrown == true);
}
END_TEST

int main(void)
{
    TCase *tc = tcase_create("ascii");
    tcase_add_test(tc, test_constructor);
    tcase_add_test(tc, test_parse);
    tcase_add_test(tc, test_parse_bad_length_short);
    tcase_add_test(tc, test_parse_bad_length_long);
    tcase_add_test(tc, test_parse_bad_checksum);
    tcase_add_test(tc, test_parse_bad_digit);

    Suite* s = suite_create("Header record container class");
    suite_add_tcase(s, tc);

    SRunner* sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    int number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return number_failed;
}
