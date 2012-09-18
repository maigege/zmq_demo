/*
 * main.cpp
 *
 *  Created on: Dec 6, 2011
 *      Author: Administrator
 */
#include <iostream>
#include <cstdlib>
#include "Server.hpp"

using namespace std;

int main(void)
{
	Server comm_obj(5558);

	sock_handle socket = comm_obj.CommBind();

	while (1) {
		string addr;
		comm_obj.CommRecv(socket, addr);

		string empty;
		comm_obj.CommRecv(socket, empty);

		string req_str;
		comm_obj.CommRecv(socket, req_str);

		cout << "come from[ " << addr << "]: " << req_str << endl;

		comm_obj.CommSend(socket, addr, COMM_SNDMORE);
		comm_obj.CommSend(socket, empty, COMM_SNDMORE);

		string resp_str = "end";
		resp_str += ": -->";
		resp_str += addr;  //addr;
		comm_obj.CommSend(socket, resp_str);
	}

	return 0;
}

