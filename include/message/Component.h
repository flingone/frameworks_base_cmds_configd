/*
 * component.h
 *
 *  Created on: 2014-7-1
 *      Author: manson
 */

#ifndef COMPONENT_H_
#define COMPONENT_H_

#include "json/json.h"
#include <string>

const std::string KEY_NAME = std::string("name");
const std::string KEY_VERSION = std::string("version");
const std::string KEY_PLATFORM = std::string("platform");

class Component {
private:
	std::string m_szName;
	std::string m_szVersion;
	std::string m_szPlatform;

	Component();

public:
	virtual ~Component();
	void Log();
	std::string getName();
	std::string getVersion();
	std::string getPlatform();

	static Component * Load(Json::Value value);
};

typedef Component Sender;
typedef Component Receiver;

#endif /* COMPONENT_H_ */
