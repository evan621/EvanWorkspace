#ifndef _SCTP_SOCKET_
#define _SCTP_SOCKET_

#include "SctpNotification.hpp"
#include "stdio.h"
#include "SctpMessage.hpp"
#include "common.hpp"
#include <memory>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <errno.h>


#define SERVER_SCTP_SOCKET 80
#define CLIENT_SCTP_SOCKET 81

class SctpSocket
{
public:
    SctpSocket(std::string localIp, uint32_t port, int socketType);
    SctpSocket(int fd);
    ~SctpSocket();
    
    int accept();
    std::unique_ptr<SctpMessageEnvelope> read();
    void write();
    int socket_fd(){return sock_fd;}
private:
    int sock_fd;
    
    void connect();
};


#endif
