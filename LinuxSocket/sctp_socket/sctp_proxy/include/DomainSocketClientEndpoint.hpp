#ifndef _DOMAIN_SOCKET_CLIENT_ENDPOINT
#define _DOMAIN_SOCKET_CLIENT_ENDPOINT

#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <string.h>
#include <memory>
#include <vector>
#include "IoMultiplex.hpp"
#include "spdlog/spdlog.h"
#include "common.hpp"


class DomainSocketClientEndpoint
{
public:
    DomainSocketClientEndpoint(const char *servername, std::shared_ptr<IoMultiplex> multiRecv, std::shared_ptr<spdlog::logger> logger);
    ~DomainSocketClientEndpoint();
    
    void send_msg(std::vector<char> msg);
    void register_handler(DomainSocketCallBack handler) {msg_handler = handler;}
private:
    int conn_fd;
    std::shared_ptr<spdlog::logger> logger;
    std::shared_ptr<IoMultiplex> io_multi;
    DomainSocketCallBack msg_handler;
    
    int domain_connect(const char *servername);
    void domain_close();
    void domain_receive(int sock_fd);
};


#endif
