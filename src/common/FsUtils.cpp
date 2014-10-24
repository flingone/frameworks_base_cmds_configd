/*
 * FsUtils.cpp
 *
 *  Created on: 2014-7-1
 *      Author: manson
 */

#include "common/FsUtils.h"

#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <sstream>

int SetFdNonBlock(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

std::string ReadFile(const std::string &path) {
	std::ifstream in;
	in.open(path.c_str(), std::ios::in);
	std::string line;
	std::stringstream ss;
	while (!in.eof()) {
		getline(in, line);
		ss << line << std::endl;
	}
	in.close();
	return ss.str();
}

bool WriteFile(const std::string &path, const std::string &content) {
	std::ofstream outfile(path.c_str(), std::ios::out);
	if (!outfile || !outfile.is_open()) {
		return false;
	}
	outfile << content;
	outfile.flush();
	outfile.close();
	return true;
}

bool IsExist(const std::string &path) {
	return access(path.c_str(), 0) == 0;
}
