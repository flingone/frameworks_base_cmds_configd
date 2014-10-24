#include "platform/PropertyManager.h"

using namespace std;

PropertyManager::PropertyManager() {
}

PropertyManager::~PropertyManager() {
}

string PropertyManager::GetProperty(string key, string defaultValue) {
	char property[256];
	memset(property, 0, 256);
	property_get(key.c_str(), property, defaultValue.c_str());
	return string(property);
}

void PropertyManager::SetProperty(string key, string value) {
	property_set(key.c_str(), value.c_str());
}
