//
// Communication.cpp
// implementation of communication class
//

#include <cstdio>
#include <cstring>
#include <iostream>
#include "server.hpp"

using namespace std;

sock_handle Server::CommBind(void)
{
	sock_handle socket;
	char addr_info[32] = "";

	socket = new zmq::socket_t(*context, ZMQ_XREP);

	sprintf(addr_info, "tcp://*:%d", service_port);

	try {
		socket->bind(addr_info);
	} catch (zmq::error_t e) {
		cout << "Connect error: %s" << e.what() << endl;
		return NULL;
	}

	return socket;
}

int Server::CommSend(sock_handle connect_handle, const string &out_str, int flags)
{
	zmq::message_t msg(out_str.size() );

//	memset(msg.data(), 0, out_str.size()+1);
	memcpy(msg.data(), out_str.data(), out_str.size() );

	try {
		int try_times = 0;
		while (!connect_handle->send(msg, flags) && try_times < 3) {
			if (zmq_errno() != EAGAIN)
				return (-1);
			++try_times;
		}
	} catch (zmq::error_t e) {
		cout << "Send error: %s" << e.what() << endl;
		return (-1);
	}

	return 0;
}

int Server::CommRecv(sock_handle connect_handle, string &in_str, int flags)
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
		cout << "Recv error: %s" << e.what() << endl;
		return (-1);
	}

	in_str.assign( static_cast<string::value_type *>(msg.data() ), msg.size() );
//	in_str = (char *)msg.data();

	return 0;
}

int Server::CommSendTimed(sock_handle connect_handle, const string &out_str, int timeout, int flags)
{
	zmq::pollitem_t item = {*connect_handle, 0, ZMQ_POLLOUT, 0};

	try {
		int rc = zmq::poll(&item, 1, timeout*1000*1000);
		if (rc == 0) {
			cerr << "timeout!!!" << endl ;
			return 1;
		} else {
			if (item.revents & ZMQ_POLLOUT) {
				zmq::message_t message(out_str.size()+1);

				memset(message.data(), 0, out_str.size()+1);
				memcpy(message.data(), out_str.data(), out_str.size() );
				connect_handle->send(message, flags);
			}
		}
	} catch (zmq::error_t e) {
		cout << "Send error: %s" << e.what() << endl;
		return (-1);
	}

	return 0;
}

int Server::CommRecvTimed(sock_handle connect_handle, string &in_str, int timeout, int flags)
{
	zmq::pollitem_t item = {*connect_handle, 0, ZMQ_POLLIN, 0};

	try {
		int rc = zmq::poll(&item, 1, timeout*1000*1000);
		if (rc == 0) {
			cerr << "timeout!!!" << endl ;
			return 1;
		} else {
			if (item.revents & ZMQ_POLLIN) {
				zmq::message_t message;

				connect_handle->recv(&message, flags);
//				in_str = (char *)message.data();
				in_str.assign( static_cast<string::value_type *>(message.data() ), message.size() );
			}
		}
	} catch (zmq::error_t e) {
		cout << "Recv error: %s" << e.what() << endl; 
		return (-1);
	}

	return 0;
}

int Server::CommClose(sock_handle connect_handle)
{
	if (connect_handle != NULL) {
		delete connect_handle;
		connect_handle = NULL;
	}

	return 0;
}


