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
#include <string.h>
#include <iostream>
#include "spdlog/spdlog.h"


#define SERVER_SCTP_SOCKET 80
#define CLIENT_SCTP_SOCKET 81

class SctpSocket
{
public:
    SctpSocket(std::string ip, uint32_t port, int socketType, std::shared_ptr<spdlog::logger> logger);
    SctpSocket(int fd, std::shared_ptr<spdlog::logger> logger);
    ~SctpSocket();
    
    int sctp_accept();
    std::unique_ptr<SctpMessageEnvelope> sctp_read();
    void sctp_write(std::vector<char> msg);
    int socket_fd(){return sock_fd;}
private:
    int sock_fd;
    std::shared_ptr<spdlog::logger> logger;

    void sctp_connect(std::string ip, uint32_t port);
    void sctp_create();
    void sctp_opt_set();
    void bind_listen_to(std::string ip, uint32_t port);
};


#endif
