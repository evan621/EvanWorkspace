#include "SctpServerEndpoint.hpp"
#include "common.h"
#include <iostream>
#include <errno.h>
#include <string.h>


SctpServerEndpoint::SctpServerEndpoint(std::string localIp, std::uint32_t port): continuepoll(true)
{
	std::cout << "[Server]: Create socket and bind to: " << localIp << ":" << port << std::endl;

	sock_op = std::make_unique<SctpSocketOperation>();
	sock_op->SetSocketOpt();
	sock_op->Bind(localIp, port);
	
	// register for callback
	sock_op->registerNotificationCb([this](std::unique_ptr<SctpMessageEnvelope> msg)
				{return onSctpNotification(std::move(msg));} );
	sock_op->registerMessageCb([this](std::unique_ptr<SctpMessageEnvelope> msg)
				{return onSctpMessages(std::move(msg));} );
	
	// create new thread for the polling of received messages
	pollThread = std::thread(&SctpServerEndpoint::ThreadHandler, this);
}


SctpServerEndpoint::~SctpServerEndpoint()
{	
	std::cout << "[Server]: SctpServerEndpoint destructor called!" << std::endl;
	
	continuepoll = false;

	pollThread.join();
}


void SctpServerEndpoint::ThreadHandler()
{
	while(continuepoll)
	{
		sock_op->StartPoolForMsg();
	}
}

int SctpServerEndpoint::onSctpNotification(std::unique_ptr<SctpMessageEnvelope> msg)
{
	std::cout << "[Poll Thread]: Notification received" << std::endl;

	notification.Print(msg->getPayload());
	return 0;
}

int SctpServerEndpoint::onSctpMessages(std::unique_ptr<SctpMessageEnvelope> msg)
{
	std::cout << "[Poll Thread]: SCTP message received: " << msg->getPayload() << std::endl;
		
	std::cout << "[sock add]: sa address: " << msg->peerIp() << std::endl;
	std::cout << "[sock add]: sa port: " << msg->peerPort() << std::endl;
	
	std::cout << "[snd rcv info]: assoid = " << msg->getAssocId() << std::endl;
	return 0;
}


void SctpServerEndpoint::SendMsg()
{
	/*
	sctp_sendmsg(sock_fd, readbuf, rd_sz, 
				(sockaddr *) &cliaddr, len, 
				sri.sinfo_ppid, (sri.sinfo_flags), sri.sinfo_stream,    //SCTP_EOF
				0, 0);
	std::cout << "[Poll Thread]: Send echo!" << std::endl;
	*/
}
