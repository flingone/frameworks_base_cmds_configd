/*
 * Component.cpp
 *
 *  Created on: 2014-7-1
 *      Author: manson
 */

#include "message/Component.h"
#include "common/Logger.h"

Component::Component() {
}

Component::~Component() {
}

Component * Component::Load(Json::Value value) {
	if (value == Json::Value::null) {
		return NULL;
	}

	//name is a required
	if (value[KEY_NAME] != Json::Value::null) {
		Component *com = new Component();
		com->m_szName = value[KEY_NAME].asString();
		com->m_szVersion = value[KEY_VERSION].asString();
		com->m_szPlatform = value[KEY_PLATFORM].asString();
		return com;
	} else {
		return NULL;
	}
}

void Component::Log() {
	LOGD("component: [name=%s], [version=%s], [platform=%s]", m_szName.c_str(),
			m_szVersion.c_str(), m_szPlatform.c_str());
}

std::string Component::getName() {
	return m_szName;
}

std::string Component::getVersion() {
	return m_szVersion;
}

std::string Component::getPlatform() {
	return m_szPlatform;
}
