/**
 *  Copyright (c) 2016, Backbone Labs Inc
 */

// Local Headers
#include "cyacd_encrypt.hpp"

// External Headers
#include <check.h>

using namespace bb;
using namespace std;

START_TEST(test_encrypt)
{
    // TODO
}
END_TEST

int main(void)
{
    TCase *tc = tcase_create("cyacd_encrypt");
    tcase_add_test(tc, test_encrypt);

    Suite* s = suite_create("Encryption functions");
    suite_add_tcase(s, tc);

    SRunner* sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    int number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return number_failed;
}
