#ifndef _WORKER
#define _WORKER

#include <stdio.h>
#include "IoMultiplex.hpp"
#include "spdlog/spdlog.h"
#include <memory>

#include "common.hpp"
#include "DomainSocket.hpp"
#include "SctpClientEndpoint.hpp"

class worker
{
public:
    worker();
    ~worker();
    
    void process();
private:
    std::shared_ptr<IoMultiplex> io_multi;
    std::shared_ptr<spdlog::logger> logger;

    std::unique_ptr<DomainSocket> master_socket_client;
    std::unique_ptr<SctpClientEndpoint> sctp_endpoint;

    bool continue_poll;

    //pollfd poll_items[2];
    std::vector<pollfd> poll_items;
    int num_poll_item;
    std::unique_ptr<SctpSocket> sctp_socket;
 
    void msg_handler();
    void send_worker_is_ready();
};

#endif
