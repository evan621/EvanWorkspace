#include "SctpServerEndpoint.hpp"
#include "common.hpp"
#include <iostream>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <cstddef>


SctpServerEndpoint::SctpServerEndpoint(std::string localIp, std::uint32_t port, std::shared_ptr<IoMultiplex> multiRecv, std::shared_ptr<spdlog::logger> logger): 
  assoInfo(nullptr), io_multi(multiRecv), logger(logger), sctp_socket_conn(nullptr)
{
    sctp_socket_listen = std::make_unique<SctpSocket>(localIp, port, SERVER_SCTP_SOCKET, logger);
      
    io_multi->register_fd(sctp_socket_listen->socket_fd(), [this](int sock_fd) 
                {   
                    SctpMsgHandler(sock_fd);
                } );  

    logger->info("SCTP Server socket create and bind to addr({}:{}), with fd {}", 
                    localIp.c_str(), port, sctp_socket_listen->socket_fd());
}


SctpServerEndpoint::~SctpServerEndpoint()
{ 
    io_multi->deregister_fd(sctp_socket_listen->socket_fd());

    if(sctp_socket_conn != nullptr)
    {
        io_multi->deregister_fd(sctp_socket_conn->socket_fd());
        sctp_socket_conn = nullptr;
    }

    logger->info("SctpServerEndpoint destructor called!");
}

void SctpServerEndpoint::register_newconn_handler(newconn_handler_callbk cb)
{
    new_conn_handler = cb;
}

int SctpServerEndpoint::SctpMsgHandler(int sock_fd)
{
    if(sock_fd == sctp_socket_listen->socket_fd())
    {
        //logger->error("SctpServerEndpoint, SctpMsgHandler with error fd")
        //return;
        int conn_sock_fd = sctp_socket_listen->sctp_accept();

        logger->info("SCTP client conn req received on listener {} with newfd {}!", sock_fd, conn_sock_fd);

        new_conn_handler(conn_sock_fd);
        
        /*

        if(sctp_socket_conn != nullptr)
        {
            io_multi->deregister_fd(sctp_socket_conn->socket_fd());
            sctp_socket_conn = nullptr;
        }

        
        sctp_socket_conn = std::make_unique<SctpSocket>(conn_sock_fd, logger);

        io_multi->register_fd(sctp_socket_conn->socket_fd(), [this](int sock_fd) 
                    {   
                        SctpMsgHandler(sock_fd);
                    } );  
                    */
    }
    else if (sock_fd == sctp_socket_conn->socket_fd())
    {
        std::unique_ptr<SctpMessageEnvelope> msg = sctp_socket_conn->sctp_read();

        if(nullptr == msg)
        {
            logger->error("Error when receive on fd {}", sock_fd);
            exit(0);
        }

        if((msg->flags())&MSG_NOTIFICATION) 
        {
            return onSctpNotification(std::move(msg));
        }
        else
        {
            return onSctpMessages(std::move(msg));
        }
    }
}

int SctpServerEndpoint::onSctpNotification(std::unique_ptr<SctpMessageEnvelope> msg)
{
  logger->info("SCTP Notification received!");
  
  sctp_notification* notification = (sctp_notification*)msg->payloadData();
      
  switch(notification->sn_header.sn_type) {
    case SCTP_ASSOC_CHANGE: 
    {
      sctp_assoc_change *sctpAssociationChange;
      sctpAssociationChange = &notification->sn_assoc_change;
      switch(sctpAssociationChange->sac_state)
      {
      case SCTP_COMM_UP:
        logger->info("Assoc change, COMMUNICATION UP! ClientAddr( IP/Port({}:{}) )", msg->peerIp()->c_str(),  msg->peerPort());

        assoInfo = std::make_unique<AssociationInfo>();
        assoInfo->ip   = msg->peerIp();
        assoInfo->port   = msg->peerPort();
        
        //AssociationInfo assInfo{msg->peerIp(), msg->peerPort(), msg->peerStream()};
        //association_list.insert(std::pair<unsigned int, AssociationInfo> (msg->getAssocId(), std::move(assInfo)))
        break;
      case SCTP_COMM_LOST:
        logger->info("Assoc change(ID=0x{}), COMMUNICATION LOST\n", sctpAssociationChange->sac_assoc_id);
        
        break;
      case SCTP_RESTART:
        logger->info("Assoc change(ID=0x{}), SCTP RESTART\n", sctpAssociationChange->sac_assoc_id);
        break;
      case SCTP_SHUTDOWN_COMP:
        // deregister from the poll
        io_multi->deregister_fd(sctp_socket_conn->socket_fd());
        logger->info("Assoc change(ID=0x{}), SHUTDOWN COMPLETE\n", sctpAssociationChange->sac_assoc_id);
        break;
      case SCTP_CANT_STR_ASSOC:
        logger->info("Assoc change(ID=0x{}), CAN'T START ASSOCIATION\n", sctpAssociationChange->sac_assoc_id);
        break;
      default:
        logger->info("Assoc chagne with unknown type (0x{})\n", sctpAssociationChange->sac_state);
        break;
      }
      break;
    }
    default:
      logger->info("Other Notification: (0x{})\n", notification->sn_header.sn_type);
      break;
  }

  //PrintAssocChange(notification);

  return 0;
}

int SctpServerEndpoint::onSctpMessages(std::unique_ptr<SctpMessageEnvelope> msg)
{
  assoInfo->stream = msg->peerStream();
  logger->info("SCTP message('{}'), size({}) received from IP/Port({}:{}) on assoc(0x{}) / stream({})\n ", 
      msg->payloadData(),
      msg->getPayload().size(),
      msg->peerIp()->c_str(),
      msg->peerPort(),
      msg->associcationId(),
      msg->peerStream());
      
  return 0;
}

void SctpServerEndpoint::SendMsg(std::vector<char> msg)
{
    sctp_socket_conn->sctp_write(std::move(msg));
}

