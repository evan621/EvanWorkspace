#include "SctpEndpoint.hpp"
#include "SctpNotification.hpp"
#include "SctpSocket.hpp"
#include "IoMultiplex.hpp"
#include <string>
#include <stdint.h>
#include <thread>
#include <memory>
#include "SctpMessage.hpp"
#include <unistd.h>
#include "spdlog/spdlog.h"
#include "common.hpp"


struct AssociationInfo{
    std::shared_ptr<std::string> ip;
    unsigned int port;
    unsigned int stream;
};

class SctpServerEndpoint: public SctpEndpoint
{
public:
    SctpServerEndpoint(std::string localIp, std::uint32_t port, std::shared_ptr<IoMultiplex> , std::shared_ptr<spdlog::logger> logger);
    ~SctpServerEndpoint();
    void SendMsg(std::vector<char> msg);
    void register_newconn_handler(newconn_handler_callbk cb);

private:
    int onSctpNotification(std::unique_ptr<SctpMessageEnvelope> msg);
    int onSctpMessages(std::unique_ptr<SctpMessageEnvelope> msg);
    void ReadUserCmd(int fd);
    
    int SctpMsgHandler(int sock_fd);

    //std::unique_ptr<SctpSocketOperation> sock_op;
    std::unique_ptr<SctpSocket> sctp_socket_listen;
    std::unique_ptr<SctpSocket> sctp_socket_conn;
    std::shared_ptr<IoMultiplex> io_multi;
    
    //std::map<unsigned int, AssociationInfo> association_list;
    std::unique_ptr<AssociationInfo> assoInfo;
    
    SctpNotification notification;
    
    std::shared_ptr<spdlog::logger> logger;
    newconn_handler_callbk new_conn_handler;
    
};
