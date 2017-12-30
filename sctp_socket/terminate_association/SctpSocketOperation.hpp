#ifndef _SCTP_SOCKET_OPERATION
#define _SCTP_SOCKET_OPERATION

#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <memory>
#include <functional>
#include "common.h"
#include "SctpMessage.hpp"
#include "SctpEndpoint.hpp"

typedef std::function<int(std::unique_ptr<SctpMessageEnvelope> msg)> Callback;

class SctpSocketOperation{
public:
	SctpSocketOperation();
	~SctpSocketOperation();
	void SetSocketOpt();
	void Bind(std::string localIp, uint32_t port);
	auto socket_fd() { return sock_fd; }
	void registerNotificationCb(Callback func) { onNotificaiton = std::move(func);}
	void registerMessageCb(Callback func) { onMessage = std::move(func); } 
	int StartPoolForMsg();
	int SctpMsgHandler(int sock_fd);

private:
	int sock_fd{-1};
	Callback onNotificaiton;
	Callback onMessage;
};


#endif