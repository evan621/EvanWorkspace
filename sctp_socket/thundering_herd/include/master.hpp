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
#include "DomainSocketClientEndpoint.hpp"
#include "DomainSocketServerEndpoint.hpp"
#include <iostream>
#include "spdlog/spdlog.h"
#include "common.hpp"


class master
{
public:
    master(std::vector<int> workers);
    ~master();
    
    void run();
private:
    void wait_until_workers_closed();
    void prepare();
    void send_terminate_to_client();
    void master_ready_forwork();
    void indicate_test_framework();
    void msg_handler(std::vector<char> msg);
    

    std::unique_ptr<SctpEndpoint> sctp_endpoint;
    std::unique_ptr<DomainSocketClientEndpoint> test_endpoint;
    std::unique_ptr<DomainSocketServerEndpoint> worker_endpoint;

    std::vector<int> workers_pid;
    
    std::shared_ptr<spdlog::logger> logger;
    std::shared_ptr<IoMultiplex> io_multi;

    bool isMasterTerminated; 

};



#endif
