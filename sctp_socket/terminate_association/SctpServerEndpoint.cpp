#include "SctpServerEndpoint.hpp"
#include "common.h"
#include <iostream>
#include <errno.h>
#include <string.h>


SctpServerEndpoint::SctpServerEndpoint(std::string localIp, std::uint32_t port): continuepoll(true), assoInfo(nullptr)
{
	std::cout << "[Poll Thread]: Create socket and bind to: " << localIp << ":" << port << std::endl;

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
	std::cout << "[Poll Thread]: SctpServerEndpoint destructor called!" << std::endl;
	
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
	
	sctp_notification* notification;

	notification = (union sctp_notification *) msg->getPayload()->c_str();
	
	switch(notification->sn_header.sn_type) {
		case SCTP_ASSOC_CHANGE: 
		{
			sctp_assoc_change *sctpAssociationChange;
			sctpAssociationChange = &notification->sn_assoc_change;
			switch(sctpAssociationChange->sac_state)
			{
			case SCTP_COMM_UP:
				std::cout << "[Poll Thread]: New association up with Id: " << msg->associcationId() << std::endl; 
				std::cout << "[Poll Thread]: New client IP: " << (*msg->peerIp()) << std::endl;
				std::cout << "[Poll Thread]: New client port: " << msg->peerPort() << std::endl;
				std::cout << "[Poll Thread]: New client trans stream: " << msg->peerStream() << std::endl;
				
				assoInfo = std::make_unique<AssociationInfo>();
				assoInfo->ip 	 = msg->peerIp();
				assoInfo->port 	 = msg->peerPort();
				assoInfo->stream = msg->peerStream();
				
				//AssociationInfo assInfo{msg->peerIp(), msg->peerPort(), msg->peerStream()};
				//association_list.insert(std::pair<unsigned int, AssociationInfo> (msg->getAssocId(), std::move(assInfo)))
				break;
			case SCTP_COMM_LOST:
				std::cout <<  "[Poll Thread]: Assoc change, COMMUNICATION LOST" << msg->associcationId() << std::endl;
				break;
			case SCTP_RESTART:
				std::cout <<  "[Poll Thread]: Assoc change, SCTP RESTART" << msg->associcationId() <<std::endl;
				break;
			case SCTP_SHUTDOWN_COMP:
				std::cout <<  "[Poll Thread]: Assoc change,SHUTDOWN COMPLETE" << msg->associcationId() <<std::endl;
				break;
			case SCTP_CANT_STR_ASSOC:
				std::cout <<  "[Poll Thread]: Assoc change,CAN'T START ASSOCIATION" << msg->associcationId() <<std::endl;
				break;
			default:
				std::cout << "[Poll Thread]: Assoc chagne with unknown type: 0x" << std::hex << sctpAssociationChange->sac_state << std::endl;
				break;
			}
			break;
		}
		default:
			std::cout << "[Poll Thread]: Other Notification: 0x" << std::hex << notification->sn_header.sn_type << std::endl;
			break;
	}

	//PrintAssocChange(notification);

	return 0;
}

int SctpServerEndpoint::onSctpMessages(std::unique_ptr<SctpMessageEnvelope> msg)
{
	std::cout << "[Poll Thread]: SCTP message received: " << *(msg->getPayload()) << std::endl;
		
	std::cout << "[Poll Thread]: Client address: " << *(msg->peerIp()) << std::endl;
	std::cout << "[Poll Thread]: Client port: " << msg->peerPort() << std::endl;
	
	std::cout << "[Poll Thread]: New association id = " << msg->associcationId() << std::endl;
	return 0;
}


void SctpServerEndpoint::SendMsg()
{
	if(nullptr == assoInfo)
	{
		std::cout << "[Main thread]: No client connected now!" << std::endl;
		return;
	}
	
	sockaddr_in clientAddr;
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = htons(assoInfo->port);
	clientAddr.sin_addr.s_addr = inet_addr(assoInfo->ip->c_str());
	
	std::string message;
	unsigned int rd_sz = 20;
	
	std::cout << "[Main thread]: Input the message echo to client: " << std::endl;
	std::cin >> message;
	
	sctp_sendmsg(sock_op->socket_fd(), message.c_str(), rd_sz, 
				(sockaddr *) &clientAddr, sizeof(sockaddr_in), 
				0, 0, assoInfo->stream,    //SCTP_EOF
				0, 0);	
}
