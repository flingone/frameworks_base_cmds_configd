/*
 * ServerSocket.h
 *
 *  Created on: 2014-1-18
 *      Author: thundersoft
 */

#ifndef SERVERSOCKET_H_
#define SERVERSOCKET_H_

#include "socket/ClientSocket.h"

const unsigned int DEFAULT_BUFFER_SIZE = 1024;

class ServerSocket {
private:
	int mId;

public:
	ServerSocket();
	virtual ~ServerSocket();

	int Init(int port);
	int GetSocketId();

	/**
	 * Accept the client socket connection
	 * return values:
	 *  < 0 : the server socket has some issus, need recreate
	 *  = 0 : non client socket connecting
	 *  > 0 : return the client socket id, success
	 */
	int AcceptTcpConnection(ClientSocket* &client);
	void Close();
};

#endif /* SERVERSOCKET_H_ */
