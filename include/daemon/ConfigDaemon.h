/*
 * ConfigDaemon.h
 *
 *  Created on: 2014-7-2
 *      Author: manson
 */

#ifndef CONFIGDAEMON_H_
#define CONFIGDAEMON_H_

#include "socket/ServerSocket.h"
#include "socket/ClientSocket.h"
#include "common/MutexQueue.h"
#include "message/MessageV1.h"

#include <map>
#include <string>
#include <sstream>

const std::string PAL_NAME = "pal";
const unsigned int SERVER_PORT = 8881;

class ConfigDaemon {
private:
	int m_TaskIdCounter;
	int m_ServerFd;

	ServerSocket *m_Server;
	std::map<int, ClientSocket*> *m_Sessions; //<clientFd, ClientSocket>
	std::map<int, ClientSocket *> *m_TaskMap; //<taskId, command-sender>

	void onMessageReceived(ClientSocket *client, std::string message);
	void wrapMessage(std::string &message);

	ClientSocket* findSessionByName(std::string name);
	ClientSocket* findSessionByFd(int fd);
	void removeSession(ClientSocket *client);
	void cleanSessions();

	ClientSocket* findTaskSenderByTaskId(int taskId);
	void removeTask(int taskId);

public:
	ConfigDaemon();
	virtual ~ConfigDaemon();

	bool Init();
	void Loop();
};

#endif /* CONFIGDAEMON_H_ */
