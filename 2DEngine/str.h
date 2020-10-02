#ifndef STR_H
#define STR_H

std::vector<int> utf8_decode(const char *string);
std::vector<unsigned short> codepoint_to_utf16(std::vector<int> codepoints);

#endif /* end of include guard: STR_H */