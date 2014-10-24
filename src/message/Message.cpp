/*
 * Message.cpp
 *
 *  Created on: 2014-7-4
 *      Author: manson
 */

#include "message/Message.h"
#include "message/MessageV1.h"
#include "common/Logger.h"

#include <string>

using namespace std;

Message::Message() {
}

Message::~Message() {
}

string Message::GetProcotolVersion() {
	return m_ProtocolVersion;
}

Message * Message::CreateMessage(string message) {
	if (message.size() == 0) {
		return NULL;
	}

	Json::Reader reader(Json::Features::strictMode());
	Json::Value value;
	if (reader.parse(message, value)) {
		if (value[KEY_PROTOCOL_VERSION] == Json::Value::null) {
			return NULL;
		}

		Message *msg = NULL;
		if (PROTOCOL_VERSION_1_0 == value[KEY_PROTOCOL_VERSION].asString()) {
			msg = MessageV1::Load(value);
			if (msg) {
				msg->m_ProtocolVersion = PROTOCOL_VERSION_1_0;
			}
		} else {
			LOGD("Unsupport message protocol : %s",
					value[KEY_PROTOCOL_VERSION].asString().c_str());
		}
		return msg;
	} else {
		return NULL;
	}
}
