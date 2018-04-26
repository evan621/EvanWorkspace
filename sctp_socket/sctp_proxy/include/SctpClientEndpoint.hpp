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
#include "SctpSocket.hpp"
#include "spdlog/spdlog.h"


class SctpClientEndpoint: public SctpEndpoint
{
public:
    SctpClientEndpoint(std::string targetIp, std::uint32_t port, std::shared_ptr<IoMultiplex> multiRecv, std::shared_ptr<spdlog::logger> logger);
    SctpClientEndpoint(std::shared_ptr<IoMultiplex> multiRecv, std::shared_ptr<spdlog::logger> logger, int fd);

    ~SctpClientEndpoint();

    void SendMsg(std::vector<char> msg);
    
    void register_newconn_handler(newconn_handler_callbk cb){}
private:
    int onSctpNotification(std::unique_ptr<SctpMessageEnvelope> msg);
    int onSctpMessages(std::unique_ptr<SctpMessageEnvelope> msg);
    int SctpMsgHandler(int sock_fd);

    std::unique_ptr<SctpSocket> sctp_socket;
    std::shared_ptr<IoMultiplex> io_multi;
    SctpNotification notification;

    std::shared_ptr<spdlog::logger> logger;
};

#endif
