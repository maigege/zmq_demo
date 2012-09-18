//
// sever.hpp
//
#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include <string>
#include "zmq.hpp"

typedef zmq::socket_t *sock_handle;

#define DEFAULT_TTL 64

#define COMM_NOBLOCK ZMQ_NOBLOCK
#define COMM_SNDMORE ZMQ_SNDMORE

struct SiteRequest {
	std::string addr;
	std::string req_str;
};

struct CenterResponse {
	std::string addr;
	std::string rep_str;
};

class Server {
public:
	inline Server(short port): service_port(port)
	{
		context = new zmq::context_t(1);
	}
	inline ~Server(void)
	{
		if (context != NULL) {
			delete context;
			context = NULL;
		}		
	}	
	sock_handle CommBind(void);
	int CommSend(sock_handle connect_handle, const std::string &out_str, int flags = 0);
	int CommRecv(sock_handle connect_handle, std::string &in_str, int flags = 0);
	int CommSendTimed(sock_handle connect_handle, const std::string &out_str, int timeout, int flags = 0);
	int	CommRecvTimed(sock_handle connect_handle, std::string &in_str, int timeout, int flags = 0);
	int CommClose(sock_handle connect_handle);

private:
//	std::string service_ip;
	short service_port;
	zmq::context_t *context;
};


#endif
