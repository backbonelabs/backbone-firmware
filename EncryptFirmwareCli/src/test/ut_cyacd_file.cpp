/**
 *  Copyright (c) 2016, Backbone Labs Inc
 */

// Local Headers
#include "cyacd_file.hpp"

// External Headers
#include <check.h>

// C Standard Headers
#include <stdio.h>

// C++ Standard Headers
#include <string>

using namespace bb;
using namespace std;

START_TEST(test_load)
{
    CyacdFile file;

    file.load("test.cyacd");

    ck_assert(file.header().silicon_id() == 0x135111A3);
    ck_assert(file.header().silicon_rev() == 0x00);
    ck_assert(file.header().checksum() == 0x00);

    ck_assert(file.data_record_count() == 219);
}
END_TEST

START_TEST(test_save)
{
    CyacdFile file;

    file.load("test.cyacd");
    file.save("test_out.cyacd");

    printf("Created test_out.cyacd.  Manually diff this with test.cyacd\n");
}
END_TEST

int main(void)
{
    TCase *tc = tcase_create("cyacd_file");
    tcase_add_test(tc, test_load);
    tcase_add_test(tc, test_save);

    Suite* s = suite_create("File parser class");
    suite_add_tcase(s, tc);

    SRunner* sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    int number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return number_failed;
}
