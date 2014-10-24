/*
 * Utils.h
 *
 *  Created on: 2014-7-3
 *      Author: manson
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <string>

const char RAND_SEED[] =
		"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

std::string GenerateRandString(int bits);

int FindFirstSymbol(std::string symbol, std::string &str);
void ShiftStringLeft(std::string &str, int length);
void ShiftStringRight(std::string &str, int length);
void ShiftString(std::string &str, int start, int length);
std::string HeadSubString(std::string &str, int length);

bool IsUTF8(const void* pBuffer, long size);
int IsTextUTF8(const char* str, long length);

#endif /* UTILS_H_ */
