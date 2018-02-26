#ifndef _TESTER_HPP
#define _TESTER_HPP

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <memory>
#include "IoMultiplex.hpp"
#include "SctpClientEndpoint.hpp"
#include "DomainSocketServerEndpoint.hpp"
#include "spdlog/spdlog.h"
#include "common.hpp"

class tester
{
public:
    tester();
    ~tester();

    void run();

private:
    std::shared_ptr<IoMultiplex> io_multi;
    std::shared_ptr<spdlog::logger> logger;
    std::unique_ptr<DomainSocketServerEndpoint> test_endpoint;
    bool continue_poll;
    
    void ReadUserCmd(int fd);
    void terminate();
    void test_case();
    void indicate_sut_to_quit();
    void test_msg_handler(std::vector<char> msg);
};

#endif
