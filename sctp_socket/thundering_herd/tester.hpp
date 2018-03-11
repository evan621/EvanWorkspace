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
#include "spdlog/spdlog.h"
#include "common.hpp"
#include "DomainSocket.hpp"

class tester
{
public:
    tester();
    ~tester();

    void run();

private:
    std::shared_ptr<IoMultiplex> io_multi;
    std::shared_ptr<spdlog::logger> logger;
    std::unique_ptr<DomainSocket> test_socket_server_listen;
    std::unique_ptr<DomainSocket> test_socket_server_connect;

    std::unique_ptr<SctpClientEndpoint> sctp_endpoint;

    std::vector<std::shared_ptr<SctpClientEndpoint>> sctp_endpoints;
    
    bool continue_poll;
    bool is_sut_ready;
    
    void ReadUserCmd(int fd);
    void terminate();
    void test_case();
    void indicate_sut_to_quit();
    void test_msg_handler(internal_msg* msg);
    void print_instruct();
    void sut_connect_handler();
};

#endif
