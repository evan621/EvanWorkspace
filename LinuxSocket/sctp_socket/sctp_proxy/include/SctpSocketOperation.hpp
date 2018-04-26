#ifndef _SCTP_SOCKET_OPERATION
#define _SCTP_SOCKET_OPERATION

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
#include "common.hpp"
#include "SctpMessage.hpp"
#include "SctpEndpoint.hpp"

class SctpSocketOperation{
public:
    SctpSocketOperation();
    ~SctpSocketOperation();
    void SetSocketOpt();
    void Bind(std::string localIp, uint32_t port);
    auto socket_fd() { return sock_fd; }
    std::unique_ptr<SctpMessageEnvelope> Receive(int sock_fd);

private:
    int sock_fd{-1};
};


#endif