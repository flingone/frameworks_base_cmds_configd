/*
 * ScreenScaler.cpp
 *
 *  Created on: 2014-6-30
 *      Author: manson
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "controller/ScreenScaler.h"

#define PROPETY_SCALE_X        			"persist.sys.scalerate_x"
#define PROPETY_SCALE_Y        			"persist.sys.scalerate_y"
#define MAIN_DISPLAY_SCALE_FILE     	"/sys/class/graphics/fb0/scale"
#define AUX_DISPLAY_SCALE_FILE      	"/sys/class/graphics/fb2/scale"
#define DEFALUT_SCREEN_SCALE 			"100"

struct scaleinfo {
	int xrate; // screen scale rate in x direction
	int yrate; // screen scale rate in y direction
};

ScreenScaler * ScreenScaler::getInstance() {
	static ScreenScaler ss;
	return &ss;
}

ScreenScaler::ScreenScaler() {
	InitSysNode();

	int x = atoi(
			m_Pm.GetProperty(PROPETY_SCALE_X, DEFALUT_SCREEN_SCALE).c_str());
	SetX(0, x);
	int y = atoi(
			m_Pm.GetProperty(PROPETY_SCALE_Y, DEFALUT_SCREEN_SCALE).c_str());
	SetY(0, y);

	//+---+---+---+---+---+---+---+---+---+---
	//1---2---3---4---5---+...+...18--19--20   //ratio
	//|---|---|---|---|---|---|---|---|---|---
	//5---10--15--20--25--+...+...90--95--100  //value
	//+---+---+---+---+---+---+---+---+---+---
	if (x >= 100) {
		m_CurrentScale = 20;
	} else {
		m_CurrentScale = x / 5;
	}
}

void ScreenScaler::SetScale(int ratio) {
	int newScaleX = ratio * 5;
	SetX(0, newScaleX);
	int newScaleY = ratio * 5;
	SetY(0, newScaleY);
	m_CurrentScale = ratio;
}

int ScreenScaler::GetScale() {
	return m_CurrentScale;
}

void ScreenScaler::InitSysNode(void) {
	memset(MainDisplaySysNode, 0, 64);
	memset(AuxDisplaySysNode, 0, 64);

	char const * const device_template = "/sys/class/graphics/fb%u/lcdcid";
	FILE *fd = NULL;
	int i = 0, id = 0;
	char name[64];

	do {
		memset(name, 0, 64);
		snprintf(name, 64, device_template, i);
		fd = fopen(name, "r");
		if (fd != NULL) {
			memset(name, 0, 64);
			fgets(name, 64, fd);
			fclose(fd);
			id = atoi(name);
			if (id == 0 && strlen(MainDisplaySysNode) == 0) {
				snprintf(MainDisplaySysNode, 64,
						"/sys/class/graphics/fb%u/scale", i);
			} else if (id == 1 && strlen(AuxDisplaySysNode) == 0) {
				snprintf(AuxDisplaySysNode, 64,
						"/sys/class/graphics/fb%u/scale", i);
			}
		}
		i++;
	} while (fd != NULL);

	if (strlen(MainDisplaySysNode) == 0) {
		strcpy(MainDisplaySysNode, MAIN_DISPLAY_SCALE_FILE);
	}
	if (strlen(AuxDisplaySysNode) == 0) {
		strcpy(AuxDisplaySysNode, AUX_DISPLAY_SCALE_FILE);
	}
}

int ScreenScaler::ReadConfig() {
	return 0;
}

void ScreenScaler::SetX(int display, int scalevalue) {
	int fd = -1;
	char property[PROPERTY_VALUE_MAX];

	fd = open(MainDisplaySysNode, O_RDWR, 0);
	if (fd < 0)
		return;
	memset(property, 0, PROPERTY_VALUE_MAX);
	sprintf(property, "xscale=%d", scalevalue);
	write(fd, property, strlen(property));
	close(fd);

	fd = open(AuxDisplaySysNode, O_RDWR, 0);
	if (fd >= 0) {
		memset(property, 0, PROPERTY_VALUE_MAX);
		sprintf(property, "xscale=%d", scalevalue);
		write(fd, property, strlen(property));
		close(fd);
	}

	memset(property, 0, PROPERTY_VALUE_MAX);
	sprintf(property, "%d", scalevalue);
	m_Pm.SetProperty(PROPETY_SCALE_X, std::string(property));
	system("sync");

}
void ScreenScaler::SetY(int display, int scalevalue) {
	int fd = -1;
	char property[PROPERTY_VALUE_MAX];

	fd = open(MainDisplaySysNode, O_RDWR, 0);
	if (fd < 0)
		return;
	memset(property, 0, PROPERTY_VALUE_MAX);
	sprintf(property, "yscale=%d", scalevalue);
	write(fd, property, strlen(property));
	close(fd);

	fd = open(AuxDisplaySysNode, O_RDWR, 0);
	if (fd >= 0) {
		memset(property, 0, PROPERTY_VALUE_MAX);
		sprintf(property, "yscale=%d", scalevalue);
		write(fd, property, strlen(property));
		close(fd);
	}

	memset(property, 0, PROPERTY_VALUE_MAX);
	sprintf(property, "%d", scalevalue);
	m_Pm.SetProperty(PROPETY_SCALE_Y, std::string(property));
	system("sync");
}

