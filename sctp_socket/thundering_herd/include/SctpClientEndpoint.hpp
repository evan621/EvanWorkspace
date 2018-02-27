#ifndef _SCTP_CLIENT_ENDPOINT_HPP
#define _SCTP_CLIENT_ENDPOINT_HPP


#include "SctpEndpoint.hpp"

#include "SctpNotification.hpp"
#include "IoMultiplex.hpp"
#include <string>
#include <stdint.h>
#include <memory>
#include <thread>
#include "SctpMessage.hpp"
#include "SctpSocketOperation.hpp"
#include "spdlog/spdlog.h"


class SctpClientEndpoint: public SctpEndpoint
{
public:
    SctpClientEndpoint(std::shared_ptr<IoMultiplex> multiRecv, std::shared_ptr<spdlog::logger> logger);
    ~SctpClientEndpoint();

    void SendMsg();
    
private:
    int onSctpNotification(std::unique_ptr<SctpMessageEnvelope> msg);
    int onSctpMessages(std::unique_ptr<SctpMessageEnvelope> msg);
    int SctpMsgHandler(int sock_fd);

    std::unique_ptr<SctpSocketOperation> sock_op;
    std::shared_ptr<IoMultiplex> io_multi;
    SctpNotification notification;

    std::shared_ptr<spdlog::logger> logger;
};

#endif
