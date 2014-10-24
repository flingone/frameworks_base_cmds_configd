/*
 * Message.h
 *
 *  Created on: 2014-7-4
 *      Author: manson
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <string>
#include "json/json.h"

class Message {
protected:
	Json::Value m_Root;
	std::string m_ProtocolVersion;

	Message();

public:

	virtual ~Message();
	virtual void Log() = 0;

	static Message * CreateMessage(std::string message);

	std::string GetProcotolVersion();
};

#endif /* MESSAGE_H_ */
