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
#include "common.h"


class master
{
public:
    master();
    ~master();
    
    void process();
    void add_worker(int pid);
    void add_workers(std::vector<int> pid_vec){workers_pid = pid_vec;}
private:
    void wait_until_workers_closed();
    void prepare();
    void send_terminate_to_client();
    void ready();

    std::unique_ptr<SctpEndpoint> sctp_endpoint;
    std::unique_ptr<DomainSocketClientEndpoint> test_endpoint;
    std::unique_ptr<DomainSocketServerEndpoint> worker_endpoint;

    std::vector<int> workers_pid;
    
    std::shared_ptr<spdlog::logger> logger;
    std::shared_ptr<IoMultiplex> io_multi;

};



#endif
