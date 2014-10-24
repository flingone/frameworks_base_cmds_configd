/*
 * ScreenScaler.h
 *
 *  Created on: 2014-6-30
 *      Author: manson
 */

#ifndef SCREENSCALER_H_
#define SCREENSCALER_H_

#include "platform/PropertyManager.h"

class ScreenScaler {
public:
	virtual ~ScreenScaler() {
	}

	static ScreenScaler * getInstance();
	void SetX(int display, int scalevalue);
	void SetY(int display, int scalevalue);

	void SetScale(int ratio);
	int GetScale();
private:
	ScreenScaler();

	int ReadConfig();
	void InitSysNode();
	char MainDisplaySysNode[64];
	char AuxDisplaySysNode[64];

	//ratio 1:20
	int m_CurrentScale;
	PropertyManager m_Pm;
};

#endif /* SCREENSCALER_H_ */
