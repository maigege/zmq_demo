
#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include <string>
#include <stdexcept>
#include "zmq.hpp"

typedef zmq::socket_t *sock_handle;
typedef zmq::socket_t sock_type;

#define DEFAULT_TTL 64

#define COMM_NOBLOCK ZMQ_NOBLOCK
#define COMM_SNDMORE ZMQ_SNDMORE

class Client {
public:
	inline Client(const char *addr, short port): center_ip(addr), center_port(port)
	{
		try {
			context = new zmq::context_t(1);
		} catch (std::bad_alloc e) {
			throw std::runtime_error("Create new zmq context error!");
		}
	}
	inline ~Client(void)
	{
		if (context != NULL) {
			delete context;
			context = NULL;
		}
	}
	sock_handle CommConnect(const char *self_id);
	int CommSend(sock_handle connect_handle, const std::string &out_str, int flags = 0);
	int CommRecv(sock_handle connect_handle, std::string &in_str, int flags = 0);
	int CommSendTimed(sock_handle connect_handle, const std::string &out_str, int timeout, int flags = 0);
	int	CommRecvTimed(sock_handle connect_handle, std::string &in_str, int timeout, int flags = 0);
	int CommClose(sock_handle connect_handle);

private:
	std::string center_ip;
	short center_port;
	zmq::context_t *context;
};

#endif /* __CLIENT_HPP__ */
