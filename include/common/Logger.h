#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <utils/Log.h>

#define LOG_TAG			"[ CONFIGD ]"

void CUSTLOG(const char * _Format, ...);
#define OTADLOG
#ifndef OTADLOG
#define LOGD  CUSTLOG
#define LOGI   CUSTLOG
#define LOGE  CUSTLOG
#else
#define LOGD  ALOGD
#define LOGI   ALOGI
#define LOGE  ALOGE
#endif
#endif
