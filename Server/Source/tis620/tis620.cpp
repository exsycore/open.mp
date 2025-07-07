#include "tis620.hpp"

const unsigned char thai_unicode_to_tis620[128] = {
	0x00, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, // 0x0E00-0x0E07
	0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, // 0x0E08-0x0E0F
	0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, // 0x0E10-0x0E17
	0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, // 0x0E18-0x0E1F
	0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, // 0x0E20-0x0E27
	0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, // 0x0E28-0x0E2F
	0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, // 0x0E30-0x0E37
	0xD8, 0xD9, 0xDA, 0x00, 0x00, 0x00, 0x00, 0xDF, // 0x0E38-0x0E3F
	0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, // 0x0E40-0x0E47
	0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, // 0x0E48-0x0E4F
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, // 0x0E50-0x0E57
	0xF8, 0xF9, 0xFA, 0xFB, 0x00, 0x00, 0x00, 0x00, // 0x0E58-0x0E5F
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x0E60-0x0E67
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x0E68-0x0E6F
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x0E70-0x0E77
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // 0x0E78-0x0E7F
};

int tis620::decode_utf8(const unsigned char* utf8, int len, unsigned int* codepoint)
{
	if (len <= 0)
		return 0;

	unsigned char first = utf8[0];

	// ASCII (0-127)
	if (first < 0x80)
	{
		*codepoint = first;
		return 1;
	}

	// 2-byte sequence (110xxxxx 10xxxxxx)
	if ((first & 0xE0) == 0xC0)
	{
		if (len < 2 || (utf8[1] & 0xC0) != 0x80)
			return -1;
		*codepoint = ((first & 0x1F) << 6) | (utf8[1] & 0x3F);
		return 2;
	}

	// 3-byte sequence (1110xxxx 10xxxxxx 10xxxxxx)
	if ((first & 0xF0) == 0xE0)
	{
		if (len < 3 || (utf8[1] & 0xC0) != 0x80 || (utf8[2] & 0xC0) != 0x80)
			return -1;
		*codepoint = ((first & 0x0F) << 12) | ((utf8[1] & 0x3F) << 6) | (utf8[2] & 0x3F);
		return 3;
	}

	// 4-byte sequence (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)
	if ((first & 0xF8) == 0xF0)
	{
		if (len < 4 || (utf8[1] & 0xC0) != 0x80 || (utf8[2] & 0xC0) != 0x80 || (utf8[3] & 0xC0) != 0x80)
			return -1;
		*codepoint = ((first & 0x07) << 18) | ((utf8[1] & 0x3F) << 12) | ((utf8[2] & 0x3F) << 6) | (utf8[3] & 0x3F);
		return 4;
	}

	return -1; // Invalid UTF-8
}

unsigned char tis620::unicode_to_tis620(unsigned int codepoint)
{
	if (codepoint <= 0x7F)
		return (unsigned char)codepoint;

	if (codepoint >= 0x0E00 && codepoint <= 0x0E7F)
	{
		unsigned char tis = thai_unicode_to_tis620[codepoint - 0x0E00];
		return tis != 0 ? tis : '?';
	}

	return '?';
}

int tis620::utf8_to_tis620(const char* utf8_input, char* tis620_output, int output_size)
{
	if (!utf8_input || !tis620_output || output_size <= 0)
		return -1;

    if (utf8_input[0] == '\0') // <== เช็คถ้าว่าง
	{
		tis620_output[0] = '\0';
		return 0;
	}

	const unsigned char* input = (const unsigned char*)utf8_input;
	int input_len = strlen(utf8_input);
	int input_pos = 0;
	int output_pos = 0;

	while (input_pos < input_len && output_pos < output_size - 1)
	{
		unsigned int codepoint;
		int bytes_consumed = decode_utf8(input + input_pos, input_len - input_pos, &codepoint);

		if (bytes_consumed <= 0)
		{
			// Invalid UTF-8 sequence, skip one byte
			input_pos++;
			continue;
		}

		unsigned char tis_char = unicode_to_tis620(codepoint);
		tis620_output[output_pos] = tis_char;
		input_pos += bytes_consumed;
		output_pos++;
	}

	// Null terminate first
	tis620_output[output_pos] = '\0';

	// Apply color fixing
	char OutputFix[2000]; // Increased buffer size
	Fixcolour::fixcolour_msg(tis620_output, OutputFix);

	// Copy back with size check
	int fix_len = strlen(OutputFix);
	if (fix_len >= output_size)
	{
		fix_len = output_size - 1;
	}
	strncpy(tis620_output, OutputFix, fix_len);
	tis620_output[fix_len] = '\0';

	return fix_len;
}

bool tis620::is_valid_utf8(const char* str)
{
	if (!str)
		return false;

	const unsigned char* bytes = (const unsigned char*)str;
	int len = strlen(str);
	int pos = 0;

	while (pos < len)
	{
		unsigned int codepoint;
		int consumed = decode_utf8(bytes + pos, len - pos, &codepoint);

		if (consumed <= 0)
			return false;

		pos += consumed;
	}
	return true;
}