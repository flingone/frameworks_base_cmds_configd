#include "common/Logger.h"
#include "daemon/ConfigDaemon.h"
#include "socket/ClientSocket.h"
#include "message/MessageV1.h"
#include "common/Utils.h"

#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <map>
#include <string>
#include <signal.h>
#include <string>
#include <sstream>
#include <queue>

using namespace std;

const bool TEST = false;

void exit_handler(int sig);
ConfigDaemon *g_Daemon = NULL;

void * thread(void * argv);

int main() {
	if (TEST) {
		pthread_t id;
		pthread_create(&id, NULL, thread, NULL);
	}

	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_handler = exit_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGINT, &act, NULL);
	sigaction(SIGKILL, &act, NULL);

	while (true) {
		ConfigDaemon *daemon = new ConfigDaemon();
		if (daemon->Init()) {
			daemon->Loop();
		}
		delete daemon;
		LOGD("restart daemon!!!");
	}
	return 0;
}

void exit_handler(int sig) {
	if (g_Daemon) {
		delete g_Daemon;
	}
	LOGD("exit_handler receiver signal : %d", sig);
	exit(0);
}

const string regist_msg =
		"{\"protocol_version\":\"1.0\",\"message_type\":\"register\",\"meta\":{\"reply\":false},\"data\":{\"name\":\"pal\"}}";
const string regist_msg_zn =
		"{\"protocol_version\":\"1.0\",\"message_type\":\"register\",\"meta\":{\"reply\":false, \"connection_mode\":\"single\"},\"data\":{\"name\":\"李劲松\"}}";
const string scale_msg =
		"{\"protocol_version\":\"1.0\",\"message_type\":\"command\",\"meta\":{\"reply\":false},\"data\":{\"command\":\"scale\",\"parameters\":{\"ratio\":10}}}";
const string sync_call_msg =
		"{\"protocol_version\":\"1.0\",\"message_type\":\"command\",\"meta\":{\"reply\":true},\"data\":{\"command\":\"test_1\",\"parameters\":{\"test_1\":10}}}";
const string reply_msg =
		"{\"protocol_version\":\"1.0\",\"message_type\":\"reply\",\"meta\":{\"reply\":false, \"task_id\":1},\"data\":{\"command\":\"test_2\",\"parameters\":{\"test_2\":10}}}";
const string forward_msg =
		"{\"protocol_version\":\"1.0\",\"message_type\":\"forward\",\"meta\":{\"reply\":false},\"data\":{\"name\":\"pal1111\"}}";
const string get_scale_msg = "{\"data\":{\"command\":\"query\",\"type\":\"device_scale\"},\"message_type\":\"command\",\"meta\":{\"reply\":true},\"protocol_version\":\"1.0\"}";

void * thread(void * argv) {
	sleep(3);

	stringstream ss;

	ClientSocket *client = new ClientSocket();
	client->SetName("Test Client");
	int clientFd = client->Connect("127.0.0.1", 8881, true);
	if (clientFd > 0) {
		LOGD("TEST client connected, fd = %d", clientFd);
	}

	ss << get_scale_msg.size();
	ss << ":";
	ss << get_scale_msg;
	client->WriteData(ss.str());
	ss.clear();
	ss.str("");

	queue<string> q;
	int res = client->ReadData(q);
	LOGD("res size ====== %d", res);

//	sleep(3);
//	ClientSocket *client2 = new ClientSocket();
//	client2->SetName("Test Client2");
//	int clientFd2 = client2->Connect("127.0.0.1", 8881, true);
//	if (clientFd2 > 0) {
//		LOGD("TEST2 client connected, fd = %d", clientFd2);
//	}
//
//	sleep(3);
//	ss << regist_msg_zn.size();
//	ss << ":";
//	ss << regist_msg_zn;
//	client2->WriteData(ss.str());
//	ss.clear();
//	ss.str("");

//	delete client;

	while (true) {
		sleep(10);
	}

	return NULL;
}
