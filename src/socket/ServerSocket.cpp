/*
 * ServerSocket.cpp
 *
 *  Created on: 2014-1-18
 *      Author: thundersoft
 */

#include "socket/ServerSocket.h"
#include "common/Logger.h"
#include "common/FsUtils.h"
#include "socket/ifaddrs.h"

#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

ServerSocket::ServerSocket() {
	mId = 0;
}

ServerSocket::~ServerSocket() {
	Close();
}

int ServerSocket::GetSocketId() {
	return mId;
}

int ServerSocket::AcceptTcpConnection(ClientSocket* &client) {
	if (mId <= 0) {
		return -1;
	}

	struct sockaddr_storage cliAddr; // Client address
	socklen_t cliAddrLen = sizeof(cliAddr);

	LOGD("SocketServer : AcceptTcpConnection.....");
	int cliFd = accept(mId, (struct sockaddr *) &cliAddr, &cliAddrLen);
	LOGD("SocketServer : accept client = %d", cliFd);

	//------manson-----//
	//usually, a non-negative value returned by accept means success.
	//but fd 0 is the fd of stdin, we should not use stdin in socket.
	//so we trade 0 as an unavailable fd.

	//-----keep original design------//
	if (cliFd < 0) {
		LOGE("SocketServer : accept error - clientFd=%d, "
				"error=%d:%s", cliFd, errno, strerror(errno));
		return -1;
	} else if (cliFd == 0) {
		return 0;
	}

	client = new ClientSocket(cliFd);
	return cliFd;
}

void ServerSocket::Close() {
	if (mId > 0) {
		close(mId);
		mId = -1;
	}
}

int ServerSocket::Init(int port) {
	LOGD("ServerSocket : port=%d", port);
	char strPort[10] = { 0 };
	sprintf(strPort, "%d", port);

	struct addrinfo addrCriteria;
	memset(&addrCriteria, 0, sizeof(addrCriteria));
	addrCriteria.ai_family = AF_UNSPEC;
	addrCriteria.ai_flags = AI_PASSIVE;
	addrCriteria.ai_socktype = SOCK_STREAM;
	addrCriteria.ai_protocol = IPPROTO_TCP;

	// List of server addresses
	struct addrinfo *servAddr;
	int rtnVal = getaddrinfo(NULL, strPort, &addrCriteria, &servAddr);
	if (rtnVal != 0 || servAddr == NULL) {
		LOGE("ServerSocket : getaddrinfo failed! servAddr=%p, error=%d:%s",
				servAddr, errno, strerror(errno));
		return -1;
	}

	struct addrinfo *addr = 0;
	int sockFd = 0;
	int err = 0;
	for (addr = servAddr; addr != 0; addr = addr->ai_next) {
		int reuse_on = 1;
		int nodelay = 1;
		int keep_alive = 1;
		int keepIdle = 60;
		int keepInterval = 5;
		int keepCount = 3;
		sockFd = socket(servAddr->ai_family, servAddr->ai_socktype,
				servAddr->ai_protocol);
		if (sockFd < 0) {
			LOGE("ServerSocket : create failed, errno = %d:%s",
			errno, strerror(errno));
			err = errno;
			continue;
		} else if ((setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &reuse_on,
				sizeof(reuse_on))) < 0) {
			LOGE("ServerSocket : reuse %d failed, errer = %d:%s", sockFd,
			errno, strerror(errno));
			err = errno;
		} else if ((setsockopt(sockFd, IPPROTO_TCP, TCP_NODELAY, &nodelay,
				sizeof(nodelay))) < 0) {
			LOGE("ServerSocket :  nodelay %d failed, errer = %d:%s", sockFd,
			errno, strerror(errno));
			err = errno;
		} else if ((setsockopt(sockFd, SOL_SOCKET, SO_KEEPALIVE,
				(void*) &keep_alive, sizeof(keep_alive))) != 0) {
			LOGE("ServerSocket : keepalive %d failed, errer = %d: %s", sockFd,
			errno, strerror(errno));
			err = errno;
		} else if (bind(sockFd, servAddr->ai_addr, servAddr->ai_addrlen)) {
			err = errno;
			char addressBuffer[INET6_ADDRSTRLEN];
			inet_ntop(servAddr->ai_family, servAddr, addressBuffer,
			INET6_ADDRSTRLEN);
			LOGE("ServerSocket : bind %d failed, addr=%s, error=%d:%s", sockFd,
					addressBuffer, err, strerror(err));
		} else if (listen(sockFd, SOMAXCONN)) {
			err = errno;
			LOGE("ServerSocket : listen %d failed, errno=%d:%s", sockFd,
			errno, strerror(errno));
		} else {
			SetFdNonBlock(sockFd);
			err = 0;
			break;
		}
		close(sockFd);
		sockFd = 0;
	}
	freeaddrinfo(servAddr);

	if (err > 0 && sockFd <= 0) {
		LOGE("ServerSocket : Init failed, errno = %d:%s", err,
				(char *) strerror(err));
		errno = err;
		return -1;
	} else if (err == 0 && sockFd == 0) {
		LOGE("ServerSocket : Init failed without error");
		return 0;
	}

	mId = sockFd;
	LOGD("ServerSocket : Init success, sockFd = %d", sockFd);
	return sockFd;
}
