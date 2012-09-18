/*
 * Client.cpp
 *
 *  Created on: Nov 24, 2011
 *      Author: Administrator
 */

#include <cstdio>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include "client.hpp"

using namespace std;

sock_handle Client::CommConnect(const char *self_id)
{
	sock_handle socket = NULL;
	char addr_info[32] = "";

	try {
		socket = new sock_type(*context, ZMQ_REQ);
	} catch (bad_alloc e) {
		cout << "Create new zmq socket error: " << e.what() << endl;
		return NULL;
	}

	sprintf(addr_info, "tcp://%s:%d", center_ip.data(), center_port);

	try {
		socket->setsockopt(ZMQ_IDENTITY, self_id, strlen(self_id) );

		int linger = 0;
		socket->setsockopt(ZMQ_LINGER, &linger, sizeof(linger) );

		socket->connect(addr_info);
	} catch (zmq::error_t e) {
		cout << "Connect error: " << e.what() << endl;
		return NULL;
	}

	return socket;
}

int Client::CommSend(sock_handle connect_handle, const string &out_str, int flags)
{
	zmq::message_t msg(out_str.size() );

	memcpy(msg.data(), out_str.data(), out_str.size() );
	
	try {
		int try_times = 0;
		while (!connect_handle->send(msg, flags) && try_times < 3) {
			if (zmq_errno() != EAGAIN)
				return (-1);
			++try_times;
		}
	} catch (zmq::error_t e) {
		cout << "Send error: " << e.what() << endl;
		return (-1);
	}

	return 0;
}

int Client::CommRecv(sock_handle connect_handle, string &in_str, int flags)
{
	zmq::message_t msg;

	try {
		int try_times = 0;
		while (!connect_handle->recv(&msg, flags) && try_times < 3) {
			if (zmq_errno() != EAGAIN)
				return (-1);
			++try_times;
		}
	} catch (zmq::error_t e) {
		cout << "Recv error: " << e.what() << endl;
		return (-1);
	}

	in_str.assign( static_cast<string::value_type *>(msg.data() ), msg.size() );

	return 0;
}

int Client::CommSendTimed(sock_handle connect_handle, const string &out_str, int timeout, int flags)
{
	zmq::pollitem_t item = {*connect_handle, 0, ZMQ_POLLOUT, 0};

	try {
		int rc = zmq::poll(&item, 1, timeout*1000*1000);
		if (rc == 0) {
			cerr << "send timeout!!!" << endl ;
			return 1;
		} else {
			if (item.revents & ZMQ_POLLOUT) {
				zmq::message_t message(out_str.size() );

				memcpy(message.data(), out_str.data(), out_str.size() );
				connect_handle->send(message, flags);
			}
		}
	} catch (zmq::error_t e) {
		cout << "Send error: " << e.what() << endl;
		return (-1);
	}

	return 0;
}

int Client::CommRecvTimed(sock_handle connect_handle, string &in_str, int timeout, int flags)
{
	zmq::pollitem_t item = {*connect_handle, 0, ZMQ_POLLIN, 0};

	try {
		int rc = zmq::poll(&item, 1, timeout*1000*1000);
		if (rc == 0) {
			cerr << "recv timeout!!!" << endl ;
			return 1;
		} else {
			if (item.revents & ZMQ_POLLIN) {
				zmq::message_t message;
				connect_handle->recv(&message, flags);
				in_str.assign( static_cast<string::value_type *>(message.data() ), message.size() );
			}
		}
	} catch (zmq::error_t e) {
		cout << "Recv error: " << e.what() << endl;
		return (-1);
	}

	return 0;
}

int Client::CommClose(sock_handle connect_handle)
{
	delete connect_handle;

	connect_handle = NULL;

	return 0;
}

