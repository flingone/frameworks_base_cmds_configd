#include "socket/ClientSocket.h"
#include "common/Logger.h"
#include "common/FsUtils.h"
#include "common/Utils.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <sstream>

#include <time.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

using namespace std;

#define READ_BUFFER_SIZE  4096

ClientSocket::ClientSocket(int id) :
		m_Fd(id), m_szName("anonymity") {
}

ClientSocket::~ClientSocket() {
	Close();
//	m_szName = "";
//	m_szReadBuffer = "";
}

int ClientSocket::Connect(string addr, int port, bool block) {
	if (m_Fd > 0) {
		LOGE("Socket is already connected : %d", m_Fd);
		return 0;
	}

	char strPort[10] = { 0 };
	sprintf(strPort, "%d", port);

	// Tell the system what kind(s) of address info we want
	struct addrinfo addrCriteria; // Criteria for address match
	memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
	addrCriteria.ai_family = AF_UNSPEC; // v4 or v6 is OK
	addrCriteria.ai_socktype = SOCK_STREAM; // Only streaming sockets
	addrCriteria.ai_protocol = IPPROTO_TCP; // Only TCP protocol

	// Get address(es)
	struct addrinfo *servAddr; // Holder for returned list of server addrs
	const char* pAddr = addr.size() > 0 ? addr.data() : NULL;
	int rtnVal = getaddrinfo(pAddr, strPort, &addrCriteria, &servAddr);
	if (rtnVal != 0 || servAddr == NULL) {
		LOGE("ClientSocket : getaddrinfo failed, addr=%s:%d, error=%d:%s",
				pAddr, port, errno, strerror(errno));
		return 0 - errno;
	}

	int sockFd = -1;
	int err = 0;
	struct addrinfo *addr_info;
	int nodelay = 1;
	int keepalive = 1;
	for (addr_info = servAddr; addr_info != NULL; addr_info =
			addr_info->ai_next) {
		// Create a reliable, stream socket using TCP
		sockFd = socket(addr_info->ai_family, addr_info->ai_socktype,
				addr_info->ai_protocol);
		if (sockFd < 0) {
			err = errno;
			continue; // Socket creation failed; try next address
		} else if ((setsockopt(sockFd, IPPROTO_TCP, TCP_NODELAY, &nodelay,
				sizeof(nodelay))) < 0) {
			LOGE("ClientSocket : nodelay %d failed, errer = %d:%s", sockFd,
			errno, strerror(errno));
			err = errno;
		} else if ((setsockopt(sockFd, SOL_SOCKET, SO_KEEPALIVE, &keepalive,
				sizeof(keepalive))) < 0) {
			LOGE("ClientSocket : keepalive %d failed, errer = %d:%s", sockFd,
			errno, strerror(errno));
			err = errno;
		} else if (::connect(sockFd, addr_info->ai_addr, addr_info->ai_addrlen)
				< 0) { // Establish the connection to the echo server
			err = errno;
			LOGE("ClientSocket : connect %d failed, error=%d:%s", sockFd,
			errno, strerror(errno));
		} else {
			err = 0;
			break;
		}

		close(sockFd); // Socket connection failed; try next address
		sockFd = -1;
	}
	freeaddrinfo(servAddr); // Free addrinfo allocated in getaddrinfo()

	if (sockFd <= 0) {
		LOGE("ClientSocket : connect failed to %s:%d, error=%d:%s", pAddr, port,
				err, strerror(err));
		return -1;
	}

	m_Fd = sockFd;
	if (!block) {
		SetFdNonBlock(m_Fd);
	}

	return sockFd;
}

void ClientSocket::SetName(string name) {
	m_szName = name;
}

string ClientSocket::GetName() {
	return m_szName;
}

int ClientSocket::GetSocketFd() const {
	return m_Fd;
}

bool ClientSocket::IsConnected() const {
	return m_Fd > 0 ? true : false;
}

void ClientSocket::Close() {
	if (m_Fd > 0) {
		LOGD("ClientSocket : [%s] close : [%d]", m_szName.c_str(), m_Fd);
		close(m_Fd);
		m_Fd = 0;
	}
}

int ClientSocket::ReadData(queue<string> &q) {
	char _buffer[READ_BUFFER_SIZE] = { 0 };
	memset(_buffer, 0, READ_BUFFER_SIZE);
	ssize_t numBytesRcvd = recv(m_Fd, _buffer, READ_BUFFER_SIZE, 0);
	LOGD("ClientSocket : read original data size [%d]", numBytesRcvd);
	if (numBytesRcvd <= 0) { //first read, lt 0 means client was closed
		return -1;
	}
	LOGD("ClientSocket : read original data [%s]", _buffer);
	m_szReadBuffer.append(_buffer, numBytesRcvd);
	LOGD("ClientSocket : current buffer [%s]", m_szReadBuffer.c_str());

	int counter = 0;
	while (true) {
		int index = FindFirstSymbol(":", m_szReadBuffer);
		if (index >= 0) {
			//parse message length
			string lenStr = HeadSubString(m_szReadBuffer, index).c_str();
			int length = atoi(lenStr.c_str());
			if (length <= 0) { //illegal message header
				LOGD("Illegal message header : [%s], drop!", lenStr.c_str());
				return -1;
			} else {
				//exclude header
				if ((unsigned int) length
						<= (m_szReadBuffer.size() - lenStr.size()/* message length */
						- 1/* ":" */)) {
					ShiftStringLeft(m_szReadBuffer, index + 1);
				} else {
					break;
				}
			}

			//parse message body
			string data = HeadSubString(m_szReadBuffer, length);
			ShiftStringLeft(m_szReadBuffer, length);
			q.push(data);
			counter++;
		} else {
			break;
		}
	}
	LOGD("ClientSocket : parsed %d messages this time", counter);
	LOGD("ClientSocket : after buffer parsed [%s]", m_szReadBuffer.c_str());
	return 0;
}

int ClientSocket::WriteData(string data) {
	const char *_data = data.c_str();
	int size = data.size();
	int numBytes = 0;
	do {
		int sent = send(m_Fd, _data + numBytes, size - numBytes, 0);
		if (sent < 0) {
			LOGE("ClientSocket : WriteData to %d failed, errer = %d:%s", m_Fd,
			errno, strerror(errno));
			return sent;
		}
		numBytes += sent;
	} while (numBytes < size);

	return numBytes;
}
