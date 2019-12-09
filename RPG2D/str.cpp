#include <iostream>
#include <vector>

#include "str.h"

#define MAXUNICODE 0x10FFFF

std::vector<int> utf8_decode(const char *string)
{
	static const unsigned int limits[] = {0xFF, 0x7F, 0x7FF, 0xFFFF};
	const unsigned char *unsignedString = (const unsigned char *)string;

	std::vector<int> result;

	unsigned int currentChar;
	int currentCode;
	int currentIndex = 0;

	while (true)
	{
		currentChar = unsignedString[currentIndex];
		currentCode = 0;
		if (currentChar == 0)
		{
			break;
		} else if (currentChar < 0x80) { // ascii ?
			currentCode = currentChar;
			currentIndex++;
			result.push_back(currentCode);
		} else {
			int count = 0; // to count number of continuation bytes
			while (currentChar & 0x40)
			{ // still have continuation bytes?
				count++;
				unsigned int continuationCharacter = unsignedString[currentIndex + count]; // read next byte
				if ((continuationCharacter & 0xC0) != 0x80) // not a continuation byte?
				{
					std::cout << "invalid byte sequence : \"" << string << "\".\n";
					exit(EXIT_FAILURE);
				}
				currentCode = (currentCode << 6) | (continuationCharacter & 0x3F); // add lower 6 bits from cont. byte
				currentChar <<= 1; // to test next bit
			}
			currentCode |= ((currentChar & 0x7F) << (count * 5)); // add first byte
			if (count > 3 || currentCode > MAXUNICODE || currentCode <= limits[count])
			{
				std::cout << "invalid byte sequence : \"" << string << "\".\n";
				exit(EXIT_FAILURE);
			}
			currentIndex += count + 1; // skip continuation bytes read and first byte
			result.push_back(currentCode);
		}
	}
	return result;
}

std::vector<unsigned short> codepoint_to_utf16(std::vector<int> codepoints)
{
	std::vector<unsigned short> result;

	for (int currentCodepointIndex = 0; currentCodepointIndex < codepoints.size(); currentCodepointIndex++)
	{
		int currentCodepoint = codepoints[currentCodepointIndex];
		if (currentCodepoint < 0x10000)
		{
			result.push_back((unsigned short)currentCodepoint);
		} else {
			currentCodepoint -= 0x10000;
			unsigned short firstWord = 0b1101100000000000 + (currentCodepoint >> 10);
			unsigned short secondWord = 0b1101110000000000 + (currentCodepoint & 0b1111111111);
			result.push_back(firstWord);
			result.push_back(secondWord);
		}
	}
	return result;
}