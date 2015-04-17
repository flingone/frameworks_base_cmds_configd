/*
 * ConfigDaemon.cpp
 *
 *  Created on: 2014-7-2
 *      Author: manson
 */

#include "daemon/ConfigDaemon.h"
#include "common/Logger.h"
#include "common/Utils.h"
#include "json/json.h"
#include "controller/ScreenScaler.h"
#include "controller/UsbController.h"
#include "controller/InputEventHandler.h"

#include <unistd.h>
#include <errno.h>
#include <queue>
#include <sstream>

using namespace std;

ConfigDaemon::ConfigDaemon() {
	m_Server = NULL;
	m_Sessions = NULL;
	m_TaskMap = NULL;

	m_TaskIdCounter = 0;
	m_ServerFd = 0;
}

ConfigDaemon::~ConfigDaemon() {
	if (m_Sessions) {
		map<int, ClientSocket*>::iterator it = m_Sessions->begin();
		while (it != m_Sessions->end()) {
			delete it->second;
			it++;
		}
		m_Sessions->clear();
		delete m_Sessions;
	}

	if (m_TaskMap) {
		m_TaskMap->clear();
		delete m_TaskMap;
	}

	if (m_Server) {
		delete m_Server;
	}

	m_ServerFd = 0;
	m_TaskIdCounter = 0;
}

bool ConfigDaemon::Init() {
	m_Sessions = new map<int, ClientSocket*>();
	m_TaskMap = new map<int, ClientSocket*>();
	m_Server = new ServerSocket();
	m_ServerFd = m_Server->Init(SERVER_PORT);
	return m_ServerFd > 0 ? true : false;
}

void ConfigDaemon::Loop() {
	if (!m_Server || !m_Sessions || !m_TaskMap || (m_ServerFd <= 0)) {
		LOGD("Init() must called before Loop, ABORT!!!");
		return;
	}

	LOGD("ConfigDaemon LOOP!!!");
	while (true) {
		fd_set read_set;
		FD_ZERO(&read_set);
		FD_SET(m_ServerFd, &read_set);

		int maxFd = m_ServerFd;
		map<int, ClientSocket*>::iterator it = m_Sessions->begin();
		while (it != m_Sessions->end()) {
			int fd = it->first;
			FD_SET(fd, &read_set);
			maxFd = (fd > maxFd) ? fd : maxFd;
			it++;
		}

		//select time out, 10 milliseconds
		struct timeval tv;
		tv.tv_sec = 10 / 1000;
		tv.tv_usec = (10 % 1000) * 1000;

		if (select(maxFd + 1, &read_set, NULL, NULL, &tv) > 0) {
			//TODO: optimize acceptance event handling
			if (FD_ISSET(m_ServerFd, &read_set)) { //check socket server
				ClientSocket* newSession = NULL;
				int fd = m_Server->AcceptTcpConnection(newSession);
				if (fd > 0) {
					if (!findSessionByFd(fd)) {
						m_Sessions->insert(
								pair<int, ClientSocket*>(fd, newSession));
					}
				} else if (fd == 0) {
					//do nothing???
				} else {
					LOGD("ConfigDaemon accept error, ABORT!!!");
					return;
				}
			}

			//check each session
			map<int, ClientSocket*>::iterator it = m_Sessions->begin();
			while (it != m_Sessions->end()) {
				int fd = it->first;
				ClientSocket *session = it->second;
				if (FD_ISSET(fd, &read_set)) {
					//read data
					queue<string> msgQueue;
					int res = session->ReadData(msgQueue);
					if (res < 0) {
						LOGD("ConfigDaemon read data error, close session");
						session->Close();
					} else {
						while (msgQueue.size() > 0) {
							string msg = msgQueue.front();
							LOGD("on process [%s] message : [%s]",
									IsTextUTF8(msg.c_str(), msg.size()) ?
											"UTF8" : "ASCII", msg.c_str());
							onMessageReceived(session, msg);
							msgQueue.pop();
						}
					}
				}
				it++;
			}

			//remove closed sessions
			cleanSessions();

			LOGD("ConfigDaemon select end, session size = %d",
					m_Sessions->size());
		}
	}
}

void ConfigDaemon::onMessageReceived(ClientSocket *client, string message) {
	Message *_msg = Message::CreateMessage(message);
	if (!_msg) {
		LOGD("Illegal format of message : [%s]", message.c_str());
		return;
	}

	if (PROTOCOL_VERSION_1_0 == _msg->GetProcotolVersion()) { //handle protocol 1.0
		MessageV1 *msgV1 = (MessageV1*) _msg;
		msgV1->SetOwner(client);
		if (msgV1->GetMessageType() == "command") {
			string command = msgV1->GetData()["command"].asString();
			if (command == "scale") { //handle self
				Json::Value params = msgV1->GetData()["parameters"];
				if (params != Json::Value::null) {
					int ratio = params["ratio"].asInt();
					if (10 <= ratio && ratio <= 20) {
						LOGD("ConfigDaemon handle "
								"command[\"set scale\"], ratio = %d", ratio);
						ScreenScaler::getInstance()->SetScale(ratio);
					}
				}
			} else if (command == "input_event") {
				Json::Value params = msgV1->GetData()["parameters"];
				if (params != Json::Value::null) {
					if (params["type"] != Json::Value::null
							&& params["code"] != Json::Value::null
							&& params["value"] != Json::Value::null) {
						unsigned int type = params["type"].asUInt();
						unsigned int code = params["code"].asUInt();
						unsigned int value = params["value"].asUInt();
						InputEventHander::sendEvent("/dev/input/event0", type,
								code, value);
					}
				}
			} else if (command == "set_usb_mode") {
				Json::Value params = msgV1->GetData()["parameters"];
				if (params != Json::Value::null) {
					std::string mode = params["usb"].asString();
					if (mode.size() > 0) {
						LOGD("ConfigDaemon handle "
								"command[\"set usb\"], mode = %s",
								mode.c_str());
						UsbController::getInstance()->set_usb_mode(mode);
					}
				}
			} else if ((command == "query")
					&& (msgV1->GetData()["type"] != Json::Value::null)
					&& (msgV1->GetData()["type"].asString() == "device_scale")) {
				int ratio = ScreenScaler::getInstance()->GetScale();
				LOGD("ConfigDaemon handle "
						"command[\"get device_scale\"], ratio = %d", ratio);
				msgV1->SetMessageType("reply");
				msgV1->SetReplyFLag(false);
				Json::Value data = msgV1->GetData();
				data["command"] = "~query";
				data["ratio"] = Json::Value(ratio);
				msgV1->SetData(data);

				string queryMessage = msgV1->Payload();
				wrapMessage(queryMessage);
				client->WriteData(queryMessage);
				LOGD("ConfigDaemon reply "
						"command[\"get device_scale\"], message : [%s]",
						queryMessage.c_str());
			} else if ((command == "query")
					&& (msgV1->GetData()["type"] != Json::Value::null)
					&& (msgV1->GetData()["type"].asString() == "device_info_2")) {
				int ratio = ScreenScaler::getInstance()->GetScale();
				std::string mode = UsbController::getInstance()->get_usb_mode();
				LOGD("ConfigDaemon handle "
						"command[\"get device_info_2\"], ratio = %d, mode=%s",
						ratio, mode.c_str());

				msgV1->SetMessageType("reply");
				msgV1->SetReplyFLag(false);
				Json::Value data = msgV1->GetData();
				data["command"] = "~query";
				data["ratio"] = Json::Value(ratio);
				data["usb"] = mode;
				msgV1->SetData(data);

				string queryMessage = msgV1->Payload();
				wrapMessage(queryMessage);
				client->WriteData(queryMessage);
				LOGD("ConfigDaemon reply "
						"command[\"get device_info_2\"], message : [%s]",
						queryMessage.c_str());
			} else { //forward to pal
				ClientSocket *pal = findSessionByName(PAL_NAME);
				if (pal) {
					if (msgV1->GetReplayFlag()) {
						//insert task id
						int id = (++m_TaskIdCounter);
						msgV1->SetTaskId(id);
						m_TaskMap->insert(pair<int, ClientSocket*>(id, client));
					}
					string forwardMessage = msgV1->Payload();
					LOGD("ConfigDaemon forward command :[%s]",
							forwardMessage.c_str());
					wrapMessage(forwardMessage);
					int res = pal->WriteData(forwardMessage);
					if (res <= 0) {
						LOGD("ConfigDaemon forward command failed :[%s]",
								forwardMessage.c_str());
						pal->Close();
						//TODO: notify sender forwarding failed
					}
				} else {
					LOGD("ConfigDaemon forward command failed, "
							"pal not ready, message : [%s]", message.c_str());
				}
			}
		} else if (msgV1->GetMessageType() == "reply") {
			int taskId = msgV1->GetTaskId();
			ClientSocket *taskSender = findTaskSenderByTaskId(taskId);
			if (taskSender && taskSender->IsConnected()) {
				string replyMessage = msgV1->Payload();
				LOGD("ConfigDaemon forward reply :[%s]", replyMessage.c_str());
				wrapMessage(replyMessage);
				taskSender->WriteData(replyMessage);
			}
			removeTask(taskId);
		} else if (msgV1->GetMessageType() == "register") {
			LOGD("ConfigDaemon received register :[%s]", message.c_str());
			string sessionName = msgV1->GetData()["name"].asString();
			if (sessionName.size() > 0) {
				if (CONNECTION_MODE_SINGLE == msgV1->GetMode()) {
					ClientSocket *namedSession = findSessionByName(sessionName);
					if (namedSession) {
						LOGD("ConfigDaemon : single connection register, "
								"[%s] registered before,"
								" cut the last session first.",
								sessionName.c_str());
						removeSession(namedSession);
					}
				}
				client->SetName(sessionName);
			}
		} else if (msgV1->GetMessageType() == "forward") {
			string forwardMessage = message;
			wrapMessage(forwardMessage);
			client->WriteData(forwardMessage);
		} else {
			LOGD("ConfigDaemon unknow message type");
		}
	}

	delete _msg;
}

void ConfigDaemon::removeTask(int taskId) {
	map<int, ClientSocket*>::iterator it = m_TaskMap->begin();
	while (it != m_TaskMap->end()) {
		if (taskId == it->first) {
			m_TaskMap->erase(it);
			break;
		}
		it++;
	}
}

ClientSocket* ConfigDaemon::findTaskSenderByTaskId(int taskId) {
	map<int, ClientSocket*>::iterator it = m_TaskMap->begin();
	while (it != m_TaskMap->end()) {
		if (taskId == it->first) {
			return it->second ? it->second : NULL;
		}
		it++;
	}
	return NULL;
}

ClientSocket* ConfigDaemon::findSessionByFd(int fd) {
	if (fd <= 0) {
		return NULL;
	}

	map<int, ClientSocket*>::iterator it = m_Sessions->begin();
	while (it != m_Sessions->end()) {
		if (fd == it->first) {
			return it->second ? it->second : NULL;
		}
		it++;
	}
	return NULL;
}

ClientSocket* ConfigDaemon::findSessionByName(string name) {
	if (name.size() == 0) {
		return NULL;
	}

	map<int, ClientSocket*>::iterator it = m_Sessions->begin();
	while (it != m_Sessions->end()) {
		ClientSocket *client = it->second;
		if (client && (client->GetName() == name)) {
			return client;
		}
		it++;
	}
	return NULL;
}

void ConfigDaemon::cleanSessions() {
	map<int, ClientSocket*>::iterator it = m_Sessions->begin();
	while (it != m_Sessions->end()) {
		ClientSocket *session = it->second;
		if (!session->IsConnected()) {
			m_Sessions->erase(it);
			delete session;
		}
		it++;
	}
}

void ConfigDaemon::removeSession(ClientSocket *client) {
	if (!client) {
		return;
	}

	map<int, ClientSocket*>::iterator it = m_Sessions->find(
			client->GetSocketFd());
	if (it != m_Sessions->end()) {
		m_Sessions->erase(it);
		delete client;
	}
}

void ConfigDaemon::wrapMessage(string &message) {
	stringstream ss;
	ss << message.size();
	ss << ":";
	message.insert(0, ss.str());
}
