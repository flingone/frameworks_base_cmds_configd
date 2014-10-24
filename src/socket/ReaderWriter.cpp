/*
 * ReaderWriter.cpp
 *
 *  Created on: 2014-7-1
 *      Author: manson
 */

#include "socket/ReaderWriter.h"
#include "common/Logger.h"
#include <errno.h>
#include <sys/socket.h>

int ReadData(int sockFd, char *buffer, int bufferSize) {
	memset(buffer, 0, bufferSize);
	ssize_t numBytesRcvd = recv(sockFd, buffer, bufferSize, 0);
	LOGD("ReadData, read %d bytes data from %d, error=%d:%s", numBytesRcvd,
			sockFd, errno, strerror(errno));
	return (int) numBytesRcvd;
}

int WriteData(int sockFd, const char *data, int size) {
	int numBytes = 0;
	do {
		int sent = send(sockFd, data + numBytes, size - numBytes, 0);
		if (sent <= 0) {
			LOGE("WriteData failed, socket error, socket = %d, errer = %d : %s",
					sockFd, errno, strerror(errno));
			return sent;
		}
		numBytes += sent;
	} while (numBytes < size);

	return numBytes;
}

