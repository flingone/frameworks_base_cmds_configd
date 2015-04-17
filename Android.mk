LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PREBUILT_LIBS := libsupc++:libs/libsupc++.a

include $(BUILD_MULTI_PREBUILT)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= src/main.cpp \
	src/json/json_reader.cpp \
	src/json/json_value.cpp \
	src/json/json_writer.cpp \
	src/controller/ScreenScaler.cpp \
	src/controller/UsbController.cpp \
	src/controller/InputEventHandler.cpp \
	src/platform/PropertyManager.cpp \
	src/common/Logger.cpp \
	src/common/FsUtils.cpp \
	src/common/Utils.cpp \
	src/socket/ServerSocket.cpp \
	src/socket/ClientSocket.cpp \
	src/socket/ReaderWriter.cpp \
	src/message/Component.cpp \
	src/message/Message.cpp \
	src/message/MessageV1.cpp \
	src/daemon/ConfigDaemon.cpp

LOCAL_C_INCLUDES += \
	frameworks/base/cmds/configd/include \
	frameworks/base/include \
	ndk/sources/cxx_stl/gnu-libstdc++/4.6/libs/armeabi-v7a/include \
	ndk/sources/cxx_stl/gnu-libstdc++/4.6/include \

LOCAL_SHARED_LIBRARIES := libcutils libstlport # libcurl liblog

LOCAL_CFLAGS := -DHAVE_CONFIG_H -DANDROID -DEXPAT_RELATIVE_PATH -DALLOW_QUOTED_COOKIE_VALUES -DCOMPONENT_BUILD -DGURL_DLL 

LOCAL_CPPFLAGS += -fexceptions  -fno-rtti

LOCAL_STATIC_LIBRARIES := libsupc++ 

#LOCAL_LDFLAGS += -L$(prebuilt_stdcxx_PATH)/thumb  -lsupc++ 

LOCAL_MODULE:= configd
LOCAL_MODULE_PATH := $(TARGET_OUT_EXECUTABLES)
LOCAL_MODULE_TAGS := eng
include external/stlport/libstlport.mk
include $(BUILD_EXECUTABLE)
