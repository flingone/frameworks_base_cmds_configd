/*
 * Message.cpp
 *
 *  Created on: 2014-7-1
 *      Author: manson
 */

#include "message/MessageV1.h"
#include "common/Logger.h"

using namespace std;

MessageV1::MessageV1() {
	m_Owner = NULL;
}

MessageV1::~MessageV1() {
}

void MessageV1::SetMessageType(string type) {
	m_Root[KEY_MESSAGE_TYPE] = type;
}

string MessageV1::GetMessageType() {
	return m_Root[KEY_MESSAGE_TYPE].asString();
}

void MessageV1::SetTaskId(int id) {
	m_Root[KEY_META][KEY_TASK_ID] = Json::Value(id);
}

int MessageV1::GetTaskId() {
	return m_Root[KEY_META][KEY_TASK_ID].asInt();
}

void MessageV1::SetReplyFLag(bool flag) {
	m_Root[KEY_META][KEY_REPLY_FLAG] = Json::Value(flag);
}

bool MessageV1::GetReplayFlag() {
	return m_Root[KEY_META][KEY_REPLY_FLAG].asBool();
}

void MessageV1::SetOwner(ClientSocket *owner) {
	m_Owner = owner;
}

ClientSocket * MessageV1::GetOwner() {
	return m_Owner;
}

string MessageV1::GetMode() {
	return m_Root[KEY_META][KEY_CONNECTION_MODE].asString();
}

Json::Value MessageV1::GetData() {
	return m_Root[KEY_DATA];
}

void MessageV1::SetData(Json::Value data) {
	m_Root[KEY_DATA] = data;
}

Json::Value MessageV1::GetRoot() {
	return m_Root;
}

void MessageV1::SetRoot(Json::Value root) {
	m_Root = root;
}

string MessageV1::Payload() {
	return m_Root.toString();
}

void MessageV1::Log() {
	LOGD("styled Message ->\n%s", m_Root.toStyledString().c_str());
}

MessageV1 * MessageV1::Load(Json::Value value) {
	if (value == Json::Value::null) {
		return NULL;
	}

	MessageV1 * msg = new MessageV1();
	msg->m_Root = value;
	if (msg->m_Root[KEY_PROTOCOL_VERSION] == Json::Value::null
			|| msg->m_Root[KEY_MESSAGE_TYPE] == Json::Value::null
			|| msg->m_Root[KEY_META] == Json::Value::null
			|| msg->m_Root[KEY_DATA] == Json::Value::null) {
		return NULL;
	}
	return msg;
}
