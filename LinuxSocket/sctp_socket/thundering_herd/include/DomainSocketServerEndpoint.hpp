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
#include <map>
#include "common.hpp"




class DomainSocketServerEndpoint
{
public:
    DomainSocketServerEndpoint(const char *servername, std::shared_ptr<IoMultiplex> multiRecv, std::shared_ptr<spdlog::logger> logger);
    ~DomainSocketServerEndpoint();

    int get_client_num() {return conn_client_fds.size(); }

    void register_handler(DomainSocketCallBack handler) {msg_handler = handler;}
    void publish_msg(std::vector<char> msg);
    
private:
    int listen_fd;
    std::vector<int> conn_client_fds;
    std::shared_ptr<IoMultiplex> io_multi;
    std::shared_ptr<spdlog::logger> logger;

    DomainSocketCallBack msg_handler;

    int domain_listen(const char *servername);
    int domain_accept();
    void domain_close();
    void domain_receive(int sock_fd);
    

};


#endif
