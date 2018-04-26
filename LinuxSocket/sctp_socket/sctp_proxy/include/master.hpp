#ifndef _MASTER
#define _MASTER

#include <stdio.h>
#include <vector>
#include <algorithm>
#include <sys/wait.h>
#include <unistd.h>
#include <memory>
#include "SctpServerEndpoint.hpp"
#include "IoMultiplex.hpp"
#include <iostream>
#include "spdlog/spdlog.h"
#include "common.hpp"
#include "DomainSocket.hpp"
#include "SctpSocket.hpp"


class master
{
public:
    master(std::vector<int> workers);
    ~master();
    
    void run();
private:
    void wait_until_workers_closed();
    void prepare();
    void send_terminate_to_worker();
    void master_ready_forwork();
    void indicate_test_framework();
    void msg_handler(internal_msg *msg_recv);
    void woker_connect_handler(int conn_fd);
    void worker_msg_handler(int fd);

    std::unique_ptr<SctpEndpoint> sctp_endpoint;
    std::unique_ptr<DomainSocket> test_socket_client;
    std::unique_ptr<DomainSocket> worker_socket_listen;
    std::map<int, std::unique_ptr<DomainSocket>> workers_socket_connect;

    std::vector<int> workers_pid;
    
    std::shared_ptr<spdlog::logger> logger;
    std::shared_ptr<IoMultiplex> io_multi;

    bool isMasterTerminated; 

    int worker_count;

};



#endif
