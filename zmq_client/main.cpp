/*
 * main.cpp
 *
 *  Created on: Dec 6, 2011
 *      Author: Administrator
 */
#include <iostream>
#include <cstring>
#include <pthread.h>
#include <cstdio>
#include <cstdlib>
#include "client.hpp"

using namespace std;

pthread_mutex_t output_lock = PTHREAD_MUTEX_INITIALIZER;

struct WorkerParam {
	string addr;
	void *comm_obj;
};

#define RECONNECT(comm_obj, comm_handle, self_id) \
{\
	comm_obj->CommClose(comm_handle);\
	comm_handle = NULL;\
	while (comm_handle == NULL) {\
		comm_handle = comm_obj->CommConnect(self_id.data() );\
	}\
}

void *WorkThreadProc(void *param)
{
	pthread_mutex_lock(&output_lock);
	cout << "Enter WorkThreadProc" << endl;
	pthread_mutex_unlock(&output_lock);

	WorkerParam *worker_param = static_cast<WorkerParam *>(param);
	Client *c = static_cast<Client *>(worker_param->comm_obj);

	sock_handle h = c->CommConnect(worker_param->addr.data() );
	if (h == NULL) {
		return reinterpret_cast<void *>(-1);
	}

	while (1) {
		int ret = 0;
		
		ret = c->CommSendTimed(h, "ready", 5);
		if (ret > 0) {
			RECONNECT(c, h, worker_param->addr);
			continue;
		} else if (ret < 0) {
			return reinterpret_cast<void *>(-1);
		}

		string recv_str;
		ret = c->CommRecvTimed(h, recv_str, 5);
		if (ret > 0) {
			pthread_mutex_lock(&output_lock);
			cout << "Reconnecting..." << endl;
			pthread_mutex_unlock(&output_lock);
			
			RECONNECT(c, h, worker_param->addr);
		} else if (ret < 0) {
			cout << "Thread will quit..." << endl;
			return reinterpret_cast<void *>(-1);
		}

		pthread_mutex_lock(&output_lock);
		cout << recv_str << endl;
		pthread_mutex_unlock(&output_lock);
	}

	return reinterpret_cast<void *>(0);
}

int main(int argc, char **argv)
{
	int i, rc = 0;
	pthread_t th_id[5];
	WorkerParam param[5];

	if (argc < 3) {
		cout << "Usage: "<< argv[0] << " [self addr] [server ip]" << endl;

		system("pause");
		return (-1);
	}

	Client comm(argv[2], 5558);

	for (i = 0; i < 5; ++i) {
		char name[32] = {0};
		sprintf(name, "%s:worker[%d]", argv[1], i);
		param[i].comm_obj = &comm;
		param[i].addr = name;
		if (pthread_create(&th_id[i], NULL, WorkThreadProc, &param[i]) != 0) {
			cerr << "Create thread" << "[" << i << "]" << "error!" << endl;
			return (-1);
		}
	}

	for (i = 0; i < 5; ++i) {
		pthread_join(th_id[i], (void **)&rc);
	}

	system("pause");
	return 0;
}
