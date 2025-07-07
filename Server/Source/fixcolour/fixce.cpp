#include "fixce.hpp"

char* Fixcolour::subc(const char* input, int offset, int len, char* dest)
{
    int input_len = strlen(input);
    if (offset + len > input_len) return nullptr;

    strncpy(dest, input + offset, len);
    dest[len] = '\0';
    return dest;
}

int Fixcolour::shiftLeft(char text[], char color[], int Pos, int shiftBy)
{
	int count = 0;

	while (shiftBy > 0)
	{
		text[Pos + 7 - count] = text[Pos - 1 - count];
		if (!memchr(igor_spaces, text[Pos - 1 - count], sizeof(igor_spaces)))
		{
			shiftBy--;
		}
		count++;
	}

	int tempIndex = Pos - count;

	text[tempIndex] = '{';
	text[tempIndex + 1] = color[0];
	text[tempIndex + 2] = color[1];
	text[tempIndex + 3] = color[2];
	text[tempIndex + 4] = color[3];
	text[tempIndex + 5] = color[4];
	text[tempIndex + 6] = color[5];
	text[tempIndex + 7] = '}';
	return Pos - count + 7;
}

int Fixcolour::shiftRight(char text[], char color[], int Pos, int shiftBy)
{
	int count = 0;
	while (shiftBy > 0)
	{
		text[Pos - 7 + count] = text[Pos + 1 + count];
		if (!memchr(igor_spaces, text[Pos + 1 + count], sizeof(igor_spaces)))
		{
			shiftBy--;
		}
		count++;
	}
	int tempIndex = Pos + count;

	text[tempIndex - 7] = '{';
	text[tempIndex - 6] = color[0];
	text[tempIndex - 5] = color[1];
	text[tempIndex - 4] = color[2];
	text[tempIndex - 3] = color[3];
	text[tempIndex - 2] = color[4];
	text[tempIndex - 1] = color[5];
	text[tempIndex] = '}';

	return Pos + count;
}

void Fixcolour::fixcolour_msg(const char* input, char* output)
{
	// std::string text(input);
	// int len = (int)text.length();
	int len = strlen(input);

	Fixcolour fix;

	if (len)
	{
		++len;
		char* text = new char[len];
		strcpy(text, input);

		int next_space = -1;
		int last_check = 0;
		int vowel_count = 0;
		int onetime_fix_bud = 0;

		for (int i = 0, j = len; i < j; ++i)
		{
			next_space = i + 7;
			if (text[i] == '{' && next_space < j && text[next_space] == '}')
			{
				char EmbledColor[7];
				fix.subc(text, i + 1, 6, EmbledColor);
				for (int x = last_check + 1; x < i; x++)
				{
					if (memchr(fix.vowel, text[x], sizeof(vowel)))
					{
						// ิัิ -44 -47 -44
						if ((text[x] & 0xD4) == 0xD4 && text[x - 2] == -47 && text[x - 4] == -44 && onetime_fix_bud == 0)
						{
							onetime_fix_bud = 1;
							continue;
						}
                        
						if ((text[x] & 0xD4) == 0xD4 && text[x - 1] == -69 && memchr(fix.bugSaraI, text[x + 2], sizeof(bugSaraI)))
						{
							vowel_count++;
							continue;
						}

						if (memchr(fix.SaraAir, text[x], sizeof(SaraAir)) && text[x - 1] == -69)
						{
							continue;
						}

						/* if ((text[x] & 0xD4) == 0xD4 || (text[x] & 0xD5) == 0xD5 || (text[x] & 0xD6) == 0xD6 || (text[x] & 0xD7) == 0xD7)
						{
							if (((char)text[x - 1] & 0xBB) == 0xBB)
								continue;
						} */

						if (memchr(fix.spaces, text[x + 1], sizeof(spaces)) || (text[x + 1] == '{' && memchr(fix.spaces, text[x + 9], sizeof(spaces))))
						{
							if ((text[x] & 0xEC) == 0xEC)
							{
								continue;
							}
							if ((text[x] & 0xE9) == 0xE9 && (text[x - 1] & 0xD5) == 0xD5 && (text[x - 2]) == -95) // กี้
							{
								continue;
							}
						}
						vowel_count++;
					}
				}
				if (vowel_count > 0)
					fix.shiftLeft(text, EmbledColor, i, vowel_count);
				else if (vowel_count < 0)
					fix.shiftRight(text, EmbledColor, next_space, abs(vowel_count));
				last_check = next_space;
				i = next_space;
			}
		}
		strcpy(output, text);
		delete[] text;
	}
}