/**
 *  Copyright (c) 2016, Backbone Labs Inc
 */

// Local Headers
#include "cyacd_data_record.hpp"

// External Headers
#include <check.h>

// C++ Standard Headers
#include <string>

using namespace bb;
using namespace std;

// First flash line from a version of our firmware...
const string test_line = "0001F201000080002011F20100FD330200FD3302000\
8B5024B83F3088804F0F8F800800020F8B572B600250122814B824C291C25601A7080\
228048D20009F0E6F98022291C7E48D20009F0E0F9291C281C7C4FFF25CA594300161\
CAE43154040194200203701330132FB18BA18934206D05D1E2D781F78AD192F700233\
F6E704312029E5D1802201276F4B52001A806F4B6F4A1F706F4B80201A60EE226E4B8\
0041A6099236D4A1B0413606D4A3C25106080206C4A400513606B4A102610606B4A02\
2013600C226A4B1A606A4A13780343694813700378AB43283D2B43037016201378664\
D03431370654B664A1E60664B1A60F022654B12031960654B1A608022644B5201E9";

// Same as above, but with a bad bit.  I won't tell which
const string bad_line = "0001F201000080002011F20100FD330200FD3302000\
8B5024B83F3088804F0F8F800800020F8B572B600250122814B824C291C25601A7080\
228048D20009F0E6F98022291C7E48D20009F0E0F9291C281C7C4FFF25CA594300161\
CAE43154040194200203701330132FB18BA18934206D05D1E2D781F88AD192F700233\
F6E704312029E5D1802201276F4B52001A806F4B6F4A1F706F4B80201A60EE226E4B8\
0041A6099236D4A1B0413606D4A3C25106080206C4A400513606B4A102610606B4A02\
2013600C226A4B1A606A4A13780343694813700378AB43283D2B43037016201378664\
D03431370654B664A1E60664B1A60F022654B12031960654B1A608022644B5201E9";

START_TEST(test_constructor)
{
    DataRecord record;

    ck_assert(record.array_id() == 0);
    ck_assert(record.row_number() == 0);
    ck_assert(record.length() == 0);
    ck_assert(record.checksum() == 0);
}
END_TEST

START_TEST(test_parse)
{
    DataRecord record;
    record.parse(test_line);

    ck_assert(record.array_id() == 0x00);
    ck_assert(record.row_number() == 0x01F2);
    ck_assert(record.length() == 0x0100);
    ck_assert(record.checksum() == 0xE9);
    ck_assert(record.ascii() == test_line);
    ck_assert(record.is_checksum_good() == true);
}
END_TEST

START_TEST(test_parse_bad_length)
{
    // TODO
}
END_TEST

START_TEST(test_parse_bad_checksum)
{
    // TODO
}
END_TEST

START_TEST(test_checksum)
{
    DataRecord record;

    record.parse(test_line);
    ck_assert(record.is_checksum_good() == true);

    record.parse(bad_line);
    ck_assert(record.is_checksum_good() == false);
}
END_TEST

START_TEST(test_load_data)
{
    //TODO
}
END_TEST

int main(void)
{
    TCase *tc = tcase_create("data_record");
    tcase_add_test(tc, test_constructor);
    tcase_add_test(tc, test_parse);
    tcase_add_test(tc, test_parse_bad_length);
    tcase_add_test(tc, test_parse_bad_checksum);
    tcase_add_test(tc, test_checksum);
    tcase_add_test(tc, test_load_data);

    Suite* s = suite_create("Data record container class");
    suite_add_tcase(s, tc);

    SRunner* sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    int number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return number_failed;
}
