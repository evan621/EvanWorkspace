#ifndef _DOMAIN_SOCKET_SERVER_ENDPOINT
#define _DOMAIN_SOCKET_SERVER_ENDPOINT

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include "IoMultiplex.hpp"
#include "spdlog/spdlog.h"
#include <memory>

#define MAX_CONNECTION_NUMBER 10

class DomainSocketServerEndpoint
{
public:
    DomainSocketServerEndpoint(const char *servername, std::shared_ptr<IoMultiplex> multiRecv, std::shared_ptr<spdlog::logger> logger);
    ~DomainSocketServerEndpoint();
    
    void ready();
private:
    int listen_fd;
    int conn_fd;
    std::shared_ptr<IoMultiplex> io_multi;
    bool isEndPointReady;    
    std::shared_ptr<spdlog::logger> logger;
    
    int domain_listen(const char *servername);
    int domain_accept();
    void domain_close();
    void domain_receive();
    

};


#endif
