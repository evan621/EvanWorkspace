#include "common.h"
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


//#define ENABLE_ALL_NOTIFICATION

SctpClientEndpoint::SctpClientEndpoint(): continuepoll(true)
{
	std::cout << "[Client]: Create new socket: SEQPACKET " << std::endl;

	sock_op = std::make_unique<SctpSocketOperation>();
	sock_op->SetSocketOpt();
	
	// register for callback
	sock_op->registerNotificationCb(std::bind(&SctpClientEndpoint::onSctpNotification, this, std::placeholders::_1));
	sock_op->registerMessageCb(std::bind(&SctpClientEndpoint::onSctpMessages, this, std::placeholders::_1));
	
	pollThread = std::thread(&SctpClientEndpoint::ThreadHandler, this);
}

SctpClientEndpoint::~SctpClientEndpoint()
{
	// close socket;
	continuepoll = false;
	std::cout << "[Client]: Close Socket!" << std::endl;
}

void SctpClientEndpoint::ThreadHandler()
{
	while(continuepoll)
	{
		sock_op->StartPoolForMsg();
	}
}


int SctpClientEndpoint::onSctpNotification(std::unique_ptr<SctpMessageEnvelope> msg)
{
	std::cout << "[Poll Thread]: Notification received" << std::endl;

	notification.Print(msg->getPayload());
	return 0;
}

int SctpClientEndpoint::onSctpMessages(std::unique_ptr<SctpMessageEnvelope> msg)
{
	std::cout << "[Poll Thread]: SCTP message received: " << msg->getPayload() << std::endl;
		
	std::cout << "[sock add]: sa address: " << msg->peerIp() << std::endl;
	std::cout << "[sock add]: sa port: " << msg->peerPort() << std::endl;
	
	std::cout << "[snd rcv info]: assoid = " << msg->getAssocId() << std::endl;
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
	
	std::cout << "[Client]: Input message to send to server!" << std::endl;
	std::cin >> message ;

	int stream = 1;
	sctp_sendmsg(sock_op->socket_fd(), message, 20, 
				(sockaddr *)&servaddr, sizeof(servaddr), 0, 0, stream, 0, 0);
	/*	
	sctp_sendmsg(sock_fd, message, 20, 
				(sockaddr *)&servaddr, sizeof(servaddr), 0, SCTP_ABORT, stream, 0, 0);
				
	*/
}
