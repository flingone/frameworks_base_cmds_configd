/*
 * UsbController.cpp
 *
 *  Created on: 2014-9-18
 *      Author: manson
 */

#include "controller/UsbController.h"
#include "common/FsUtils.h"
#include "common/Logger.h"

const std::string CONFIG_PATH = "/data/otg.cfg";

UsbController * UsbController::getInstance() {
	static UsbController uc;
	return &uc;
}

std::string UsbController::get_usb_mode_() {
	if (!IsExist(CONFIG_PATH)) {
		return std::string("");
	}
	std::string mode = ReadFile(CONFIG_PATH);
	if (mode.size() > 0) {
		if (mode.compare(mode.size() - 1, 1, "\n") == 0) {
			mode.pop_back();
		}
	}
	return mode;
}

void UsbController::set_usb_mode_(const std::string &mode) {
	if (mode.size() <= 0) {
		return;
	}

	WriteFile(CONFIG_PATH, mode);
	if (mode == get_usb_mode_()) {
		LOGD("set usb mode ok: %s", mode.c_str());
	} else {
		LOGD("set usb mode error: %s", mode.c_str());
	}
}
