/*
 * InputEventHander.cpp
 *
 *  Created on: 2015-4-17
 *      Author: manson
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "controller/InputEventHandler.h"

InputEventHandler::InputEventHandler() {
	// TODO Auto-generated constructor stub

}

InputEventHandler::~InputEventHandler() {
	// TODO Auto-generated destructor stub
}

int InputEventHandler::sendEvent(const std::string& path, unsigned short type,
		unsigned short code, unsigned int value) {
	int ret = 0;
	ret = sendEventInternal(path, type, code, value);
	if (ret != 0) {
		return ret;
	}
	ret = sendEventInternal(path, 0, 0, 0);
	return ret;
}

int InputEventHandler::sendEventInternal(const std::string& path,
		unsigned short type, unsigned short code, unsigned int value) {
	int fd;
	int ret;
	int version;
	struct input_event event;

	fd = open(path.c_str(), O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "could not open %s, %s\n", path.c_str(),
				strerror(errno));
		return 1;
	}

	memset(&event, 0, sizeof(event));
	event.type = type;
	event.code = code;
	event.value = value;
	ret = write(fd, &event, sizeof(event));
	if (ret < sizeof(event)) {
		fprintf(stderr, "write event failed, %s\n", strerror(errno));
		return -1;
	}
	return 0;
}
