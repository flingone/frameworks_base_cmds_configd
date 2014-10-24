#ifndef _PROPERTY_H_
#define _PROPERTY_H_

#include <string>
#include <cutils/properties.h>

#define PROPERTY_KEY_MAX   				32
#define PROPERTY_VALUE_MAX  			92

class PropertyManager {
public:
	PropertyManager();
	virtual ~PropertyManager();
	std::string GetProperty(std::string key, std::string defaultValue);
	void SetProperty(std::string key, std::string value);
};

#endif
