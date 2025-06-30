#pragma once

#include <cstring>
#include <cstdlib>

extern const unsigned char thai_unicode_to_tis620[128];

class tis620
{
    public:
        static int decode_utf8(const unsigned char* utf8, int len, unsigned int* codepoint);
        static unsigned char unicode_to_tis620(unsigned int codepoint);
        static int utf8_to_tis620(const char* utf8_input, char* tis620_output, int output_size);
        static bool is_valid_utf8(const char* str);
};
