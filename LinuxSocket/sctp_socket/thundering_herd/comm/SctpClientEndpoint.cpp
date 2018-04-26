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
#include "SctpSocket.hpp"
#include <stdio.h>


//#define ENABLE_ALL_NOTIFICATION

SctpClientEndpoint::SctpClientEndpoint(std::string targetIp, std::uint32_t port, std::shared_ptr<IoMultiplex> multiRecv, std::shared_ptr<spdlog::logger> logger): 
    io_multi(multiRecv), logger(logger)
{

    sctp_socket = std::make_unique<SctpSocket>(targetIp, port, CLIENT_SCTP_SOCKET, logger);

    // register fd
    io_multi->register_fd(sctp_socket->socket_fd(), [this](int fd)
                                      { SctpMsgHandler(fd); });

    logger->info("SCTP Client construct! New sockert fd: {} is created and registered.", sctp_socket->socket_fd());
}

SctpClientEndpoint::SctpClientEndpoint(std::shared_ptr<IoMultiplex> multiRecv, std::shared_ptr<spdlog::logger> logger, int fd):    
    io_multi(multiRecv), logger(logger)
{
    sctp_socket = std::make_unique<SctpSocket>(fd, logger);

    printf("SctpClientEndpoint(%d), Create new sctp endpoint with given fd, %d\n",getpid(), fd);
    //sctp_socket->socket_fd()
    io_multi->register_fd(fd, [this](int sock_fd)
                                      { SctpMsgHandler(sock_fd); });

    printf("SctpClientEndpoint(%d), Register done\n", getpid());

    logger->info("SCTP Client construct! New sockert fd: {} is created and registered.", sctp_socket->socket_fd());
}

SctpClientEndpoint::~SctpClientEndpoint()
{
    // close socket;
    // TODO: Should find a more gracefull SCTP link shutdown procedure. 
    io_multi->deregister_fd(sctp_socket->socket_fd());
    logger->info("SCTP Client destruct!, deregister fd: {}", sctp_socket->socket_fd());
}

int SctpClientEndpoint::SctpMsgHandler(int sock_fd)
{
    printf("SCTP message handler, %d, fd, %d\n", sock_fd, sctp_socket->socket_fd());
    std::unique_ptr<SctpMessageEnvelope> msg = sctp_socket->sctp_read();

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

  printf("[Info PID = %d]: SCTP Notification receivd\n", getpid());
  
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
void SctpClientEndpoint::SendMsg(std::vector<char> msg)
{
    sctp_socket->sctp_write(std::move(msg));
}
