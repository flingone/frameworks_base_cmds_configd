/*
 * FsUtils.h
 *
 *  Created on: 2014-7-1
 *      Author: manson
 */

#ifndef FSUTILS_H_
#define FSUTILS_H_

#include <string>

int SetFdNonBlock(int fd);
std::string ReadFile(const std::string &path);
bool WriteFile(const std::string &path, const std::string &content);
bool IsExist(const std::string &path);

#endif /* FSUTILS_H_ */
