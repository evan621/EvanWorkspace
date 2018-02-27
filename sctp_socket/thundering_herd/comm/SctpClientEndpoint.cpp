#include "common.hpp"
#include <unistd.h>
#include "SctpClientEndpoint.hpp"
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <poll.h>
#include <functional>
#include "SctpSocketOperation.hpp"
#include <stdio.h>


//#define ENABLE_ALL_NOTIFICATION

SctpClientEndpoint::SctpClientEndpoint(std::shared_ptr<IoMultiplex> multiRecv, std::shared_ptr<spdlog::logger> logger): 
    io_multi(multiRecv), logger(logger)
{
    logger->info("SCTP Client construct!");

    sock_op = std::make_unique<SctpSocketOperation>();
    sock_op->SetSocketOpt();

    // register fd
    io_multi->RegisterFd(sock_op->socket_fd(), [this](int fd)
                                      { SctpMsgHandler(fd); });
}

SctpClientEndpoint::~SctpClientEndpoint()
{
    // close socket;
    logger->info("SCTP Client destruct!");
}

int SctpClientEndpoint::SctpMsgHandler(int sock_fd)
{
  std::unique_ptr<SctpMessageEnvelope> msg = sock_op->Receive(sock_fd);
  
  if(nullptr == msg)
  {
    std::cout << "[Client]: Failed received message on socket: " << sock_fd << std::endl;
    return -1;
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


int SctpClientEndpoint::onSctpNotification(std::unique_ptr<SctpMessageEnvelope> msg)
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
        logger->info("Assoc change, COMMUNICATION UP! ClientAddr IP/Port({}:{}) ", msg->peerIp()->c_str(),  msg->peerPort());

        break;
      case SCTP_COMM_LOST:
        logger->info("Assoc change(ID={}), COMMUNICATION LOST", sctpAssociationChange->sac_assoc_id);
        break;
      case SCTP_RESTART:
        logger->info("Assoc change(ID={}), SCTP RESTART", sctpAssociationChange->sac_assoc_id);
        break;
      case SCTP_SHUTDOWN_COMP:
        logger->info("Assoc change(ID={}), SHUTDOWN COMPLETE", sctpAssociationChange->sac_assoc_id);
        break;
      case SCTP_CANT_STR_ASSOC:
        logger->info("Assoc change(ID={}), CAN'T START ASSOCIATION", sctpAssociationChange->sac_assoc_id);
        break;
      default:
        logger->info("Assoc chagne with unknown type (0x{})", sctpAssociationChange->sac_state);
        break;
      }
      break;
    }
    default:
      logger->info("Other Notification: (0x{})", notification->sn_header.sn_type);
      break;
  }

  
  //notification.Print(msg->getPayload()->c_str());
  return 0;
}

int SctpClientEndpoint::onSctpMessages(std::unique_ptr<SctpMessageEnvelope> msg)
{
  logger->info("SCTP message('') received from IP/Port({}:{}) on assoc(0x{}) / stream({})", 
      msg->payloadData(),
      msg->peerIp()->c_str(),
      msg->peerPort(),
      msg->associcationId(),
      msg->peerStream());
  return 0;
}

void SctpClientEndpoint::SendMsg()
{
    //std::string message;
    char message[20];
    struct sctp_sndrcvinfo sinfo;
    struct sockaddr_in servaddr;

    bzero( (void *)&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MY_PORT_NUM);
    servaddr.sin_addr.s_addr = inet_addr( "127.0.0.1" );

    int stream = 1;
        printf("[Client]: Input message content!\n");
    std::cin >> message ;

    sctp_sendmsg(sock_op->socket_fd(), message, 20, 
                  (sockaddr *)&servaddr, sizeof(servaddr), 0, 0, stream, 0, 0);
}

void SctpClientEndpoint::SendMsg(std::vector<char> msg)
{
    
}

