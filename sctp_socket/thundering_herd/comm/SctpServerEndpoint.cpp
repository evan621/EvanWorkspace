#include "SctpServerEndpoint.hpp"
#include "common.hpp"
#include <iostream>
#include <errno.h>
#include <string.h>
#include <stdio.h>


SctpServerEndpoint::SctpServerEndpoint(std::string localIp, std::uint32_t port, std::shared_ptr<IoMultiplex> multiRecv, std::shared_ptr<spdlog::logger> logger): 
  assoInfo(nullptr), io_multi(multiRecv), logger(logger)
{
    sctp_socket_listen = std::make_unique<SctpSocket>(localIp, port, SERVER_SCTP_SOCKET, logger);
      
    io_multi->RegisterFd(sctp_socket_listen->socket_fd(), [this](int sock_fd) 
                {   
                    SctpMsgHandler(sock_fd);
                } );  

    //socket_tab.insert(std::pair<int, std::unique_ptr<SctpSocket>>(sctp_socket->socket_fd(), std::move(sctp_socket)))
    logger->info("Server Start and bind to addr({}:{})", localIp.c_str(), port);
}


SctpServerEndpoint::~SctpServerEndpoint()
{ 
    logger->info("SctpServerEndpoint destructor called!");
}

int SctpServerEndpoint::SctpMsgHandler(int sock_fd)
{
    if(sock_fd == sctp_socket_listen->socket_fd())
    {
        //logger->error("SctpServerEndpoint, SctpMsgHandler with error fd")
        //return;
        int conn_sock_fd = sctp_socket_listen->sctp_accept();

        logger->info("New SCTP client connected with fd {}!", conn_sock_fd);

        sctp_socket_conn = std::make_unique<SctpSocket>(conn_sock_fd, logger);

        io_multi->RegisterFd(sctp_socket_conn->socket_fd(), [this](int sock_fd) 
                    {   
                        SctpMsgHandler(sock_fd);
                    } );  
    }
    else if (sock_fd == sctp_socket_conn->socket_fd())
    {
        std::unique_ptr<SctpMessageEnvelope> msg = sctp_socket_conn->read();

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
        io_multi->DeRegisterFd(sctp_socket_conn->socket_fd());
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
    sctp_socket_conn->write(std::move(msg));
}

/*
void SctpServerEndpoint::SendMsg()
{
  if(nullptr == assoInfo)
  {
    std::cout << "[Server]: No client connected now!" << std::endl;
    return;
  }
  
  sockaddr_in clientAddr;
  clientAddr.sin_family = AF_INET;
  clientAddr.sin_port = htons(assoInfo->port);
  clientAddr.sin_addr.s_addr = inet_addr(assoInfo->ip->c_str());
  
  std::string message;
  unsigned int rd_sz = 20;
  
  std::cout << "[Server]: Input the message echo to client: " << std::endl;
  std::cin >> message;
  
  sctp_sendmsg(sctp_socket->socket_fd(), message.c_str(), rd_sz, 
        (sockaddr *) &clientAddr, sizeof(sockaddr_in), 
        0, 0, assoInfo->stream,    //SCTP_EOF
        0, 0);  
}
*/

