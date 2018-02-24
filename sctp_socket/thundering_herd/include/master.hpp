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
    
private:
    void wait_until_workers_closed();

    std::unique_ptr<SctpEndpoint> endpoint;
    std::unique_ptr<DomainSocketClientEndpoint> domain_endpoint;

    std::vector<int> workers_pid;
    
    std::shared_ptr<spdlog::logger> logger;
    std::shared_ptr<IoMultiplex> io_multi;

};



#endif