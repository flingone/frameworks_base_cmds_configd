/*
 * ClientSocket.h
 *
 */

#ifndef CLIENTSOCKET_H_
#define CLIENTSOCKET_H_

#include <string>
#include <queue>

const std::string MESSAGE_SEPARATOR = ":";

class ClientSocket {
private:
	int m_Fd;
	std::string m_szName;
	std::string m_szReadBuffer;

public:
	ClientSocket(int id = 0);
	virtual ~ClientSocket();

	/*
	 * create a client socket and connect to the special address
	 * return values:
	 *  < 0 : system error
	 *  = 0 : the socket already exist
	 *  > 0 : the client socket id, success
	 */
	int Connect(std::string ip, int port, bool block);

	int GetSocketFd() const;
	bool IsConnected() const;
	void Close();

	void SetName(std::string name);
	std::string GetName();

	int ReadData(std::queue<std::string> &q);
	int WriteData(std::string data);
};

#endif /* CLIENTSOCKET_H_ */
