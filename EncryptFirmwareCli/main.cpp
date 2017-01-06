/**
 *  @copyright Backbone Labs Inc
 *  @copyright December 11, 2016
 *  @copyright All rights reserved
 *
 *  @author Barry Gackle
 *  @author bdgackle@gmail.com
 */

// Local Headers
#include "cyacd_file.hpp"

// C Standard Headers
#include <cstdio>

// C++ Standard Headers
#include <string>

using namespace bb;
using namespace std;

static void print_usage()
{
    printf("Usage:\n");
    printf("cyacd_encryptor <input_file> <output_file>\n");
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        print_usage();
        return 0;
    }

    string input_filename = string(argv[1]);
    string output_filename = string(argv[2]);

    CyacdFile file;
    file.load(input_filename);
    file.encrypt();
    file.save(output_filename);
}
