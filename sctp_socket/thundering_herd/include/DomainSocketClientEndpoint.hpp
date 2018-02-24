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
#include "IoMultiplex.hpp"
#include "spdlog/spdlog.h"



class DomainSocketClientEndpoint
{
public:
    DomainSocketClientEndpoint(const char *servername, std::shared_ptr<spdlog::logger> logger);
    ~DomainSocketClientEndpoint();
    
    void send_msg();
private:
    int listen_fd;
    int conn_fd;
    std::shared_ptr<spdlog::logger> logger;
    //IoMultiplex& io_multi;
    
    int domain_connect(const char *servername);
    void domain_close();
};


#endif
