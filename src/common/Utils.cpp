/*
 * Utils.cpp
 *
 *  Created on: 2014-7-3
 *      Author: manson
 */

#include "common/Utils.h"
#include "common/Logger.h"
#include "stdlib.h"
#include "time.h"
#include <sstream>

using namespace std;

string GenerateRandString(int bits) {
	srand(time(NULL));
	stringstream ss;
	for (int i = 1; i <= bits; i++) {
		int index = rand() / (RAND_MAX / (sizeof(RAND_SEED) - 1));
		ss << RAND_SEED[index];
	}
	return ss.str();
}

int FindFirstSymbol(std::string symbol, std::string &str) {
	return str.find_first_of(symbol);
}

void ShiftStringLeft(string &str, int length) {
	ShiftString(str, 0, length);
}

void ShiftStringRight(std::string &str, int length) {
	int start = str.size() - length;
	ShiftString(str, start, length);
}

void ShiftString(std::string &str, int start, int length) {
	str.erase(start, length);
}

std::string HeadSubString(std::string &str, int length) {
	return str.substr(0, length);
}

bool IsUTF8(const void* pBuffer, long size) {
	bool IsUTF8 = true;
	unsigned char* start = (unsigned char*) pBuffer;
	unsigned char* end = (unsigned char*) pBuffer + size;
	while (start < end) {
		if (*start < 0x80) // (10000000): 值小于0x80的为ASCII字符
				{
			start++;
		} else if (*start < (0xC0)) // (11000000): 值介于0x80与0xC0之间的为无效UTF-8字符
				{
			IsUTF8 = false;
			break;
		} else if (*start < (0xE0)) // (11100000): 此范围内为2字节UTF-8字符
				{
			if (start >= end - 1)
				break;
			if ((start[1] & (0xC0)) != 0x80) {
				IsUTF8 = false;
				break;
			}
			start += 2;
		} else if (*start < (0xF0)) // (11110000): 此范围内为3字节UTF-8字符
				{
			if (start >= end - 2)
				break;
			if ((start[1] & (0xC0)) != 0x80 || (start[2] & (0xC0)) != 0x80) {
				IsUTF8 = false;
				break;
			}
			start += 3;
		} else {
			IsUTF8 = false;
			break;
		}
	}
	return IsUTF8;
}

int IsTextUTF8(const char* str, long length) {
	int i;
	int nBytes = 0; //UFT8可用1-6个字节编码,ASCII用一个字节
	unsigned char chr;
	bool bAllAscii = true; //如果全部都是ASCII, 说明不是UTF-8
	for (i = 0; i < length; i++) {
		chr = *(str + i);
		if ((chr & 0x80) != 0) { // 判断是否ASCII编码,如果不是,说明有可能是UTF-8,ASCII用7位编码,但用一个字节存,最高位标记为0,o0xxxxxxx
			bAllAscii = false;
		}
		if (nBytes == 0) { //如果不是ASCII码,应该是多字节符,计算字节数
			if (chr >= 0x80) {
				if (chr >= 0xFC && chr <= 0xFD)
					nBytes = 6;
				else if (chr >= 0xF8)
					nBytes = 5;
				else if (chr >= 0xF0)
					nBytes = 4;
				else if (chr >= 0xE0)
					nBytes = 3;
				else if (chr >= 0xC0)
					nBytes = 2;
				else {
					return false;
				}
				nBytes--;
			}
		} else { //多字节符的非首字节,应为 10xxxxxx
			if ((chr & 0xC0) != 0x80) {
				return false;
			}
			nBytes--;
		}
	}

	if (nBytes > 0) { //违返规则
		return false;
	}

	if (bAllAscii) { //如果全部都是ASCII, 说明不是UTF-8
		return false;
	}
	return true;
}
