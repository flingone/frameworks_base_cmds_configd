/*
 * UsbController.h
 *
 *  Created on: 2014-9-18
 *      Author: manson
 */

#ifndef USBCONTROLLER_H_
#define USBCONTROLLER_H_

#include <string>

class UsbController {
private:
	UsbController() {
	}

	std::string get_usb_mode_();
	void set_usb_mode_(const std::string &mode);

public:
	virtual ~UsbController() {
	}

	static UsbController * getInstance();

	std::string get_usb_mode() {
		return get_usb_mode_();
	}

	void set_usb_mode(const std::string &mode) {
		set_usb_mode_(mode);
	}
};

#endif /* USBCONTROLLER_H_ */
