/*
 * MessageV1.h
 *
 *  Created on: 2014-7-1
 *      Author: manson
 */

#ifndef MESSAGEV1_H_
#define MESSAGEV1_H_

#include "message/Message.h"
#include "message/Component.h"
#include "socket/ClientSocket.h"
#include "json/json.h"
#include <string>

const std::string PROTOCOL_VERSION_1_0 = "1.0";
const std::string CONNECTION_MODE_SINGLE = "single";
const std::string CONNECTION_MODE_MULTI = "multi";

const std::string KEY_PROTOCOL_VERSION = "protocol_version";
const std::string KEY_MESSAGE_TYPE = "message_type";
const std::string KEY_META = "meta";
const std::string KEY_REPLY_FLAG = "reply";
const std::string KEY_TASK_ID = "task_id";
const std::string KEY_CONNECTION_MODE = "connection_mode";
const std::string KEY_DATA = "data";

class MessageV1: public Message {
private:
	ClientSocket *m_Owner;

	MessageV1();

public:
	virtual ~MessageV1();

	void SetMessageType(std::string type);
	std::string GetMessageType();

	void SetTaskId(int id);
	int GetTaskId();

	void SetReplyFLag(bool flag);
	bool GetReplayFlag();

	std::string GetMode();

	void SetOwner(ClientSocket *owner);
	ClientSocket *GetOwner();

	Json::Value GetData();
	void SetData(Json::Value data);

	void SetRoot(Json::Value root);
	Json::Value GetRoot();

	std::string Payload();

	static MessageV1 * Load(Json::Value value);

	void Log();
};

#endif /* MESSAGEV1_H_ */
