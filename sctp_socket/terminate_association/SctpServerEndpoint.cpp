#include "SctpServerEndpoint.hpp"
#include "common.h"
#include <iostream>
#include <errno.h>
#include <string.h>


SctpServerEndpoint::SctpServerEndpoint(std::string localIp, std::uint32_t port): continuepoll(true), assoInfo(nullptr)
{
	sock_op = std::make_unique<SctpSocketOperation>();
	sock_op->SetSocketOpt();
	sock_op->Bind(localIp, port);
	
	// register fd
	io_multi = std::make_unique<IoMultiplex>();
	io_multi->RegisterFd(sock_op->socket_fd(), [this](int sock_fd) 
			{ 	
				if(-1==SctpMsgHandler(sock_fd))
				{
					continuepoll = false;
				}
			} );
	io_multi->RegisterFd(STDIN, [this](int fd) 
			{  ReadUserCmd(fd); } );
			
	std::cout << "[Server]: Server Start and bind to: " << localIp << ":" << port << std::endl;
	std::cout << "[Server]: Register socket fd: " << sock_op->socket_fd() << std::endl;
}

SctpServerEndpoint::~SctpServerEndpoint()
{	
	std::cout << "[Server]: SctpServerEndpoint destructor called!" << std::endl;
}

void SctpServerEndpoint::Start()
{	
	while(continuepoll)
	{
		std::cout << "=============================================" << std::endl;
		std::cout << "[Server]: Wait for new message or command..." << std::endl;
		std::cout << "[Server]: [0] Exist the client!" << std::endl; 
		std::cout << "[Server]: [1] Send new message to server!" << std::endl; 
		io_multi->Poll(); 
	}
}

void SctpServerEndpoint::ReadUserCmd(int fd)
{
	char buf;  
	read(fd, &buf, sizeof(buf));
	switch(buf)
	{
		case '0':
			std::cout << "Exit server! " << std::endl; 
			continuepoll = false;
			break;
		case '1':
			SendMsg();
			break;
		default:
			std::cout << "Unrecognized command!" << std::endl;
			break;
	}
}


int SctpServerEndpoint::SctpMsgHandler(int sock_fd)
{
	std::unique_ptr<SctpMessageEnvelope> msg = sock_op->Receive(sock_fd);
	
	if(nullptr == msg)
	{
		std::cout << "[Server]: Error when receive on fd: " << sock_fd << std::endl;
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

int SctpServerEndpoint::onSctpNotification(std::unique_ptr<SctpMessageEnvelope> msg)
{
	std::cout << "[Server]: Notification received" << std::endl;
	
	sctp_notification* notification;

	notification = (sctp_notification*)msg->payloadData();
	
	msg->print();
	
	switch(notification->sn_header.sn_type) {
		case SCTP_ASSOC_CHANGE: 
		{
			sctp_assoc_change *sctpAssociationChange;
			sctpAssociationChange = &notification->sn_assoc_change;
			switch(sctpAssociationChange->sac_state)
			{
			case SCTP_COMM_UP:
				std::cout << "[Server]: New association up with Id: " << sctpAssociationChange->sac_assoc_id << std::endl; 
				std::cout << "[Server]: New client IP: " << (*msg->peerIp()) << std::endl;
				std::cout << "[Server]: New client port: " << msg->peerPort() << std::endl;
				std::cout << "[Server]: New client trans stream: " << msg->peerStream() << std::endl;
				
				assoInfo = std::make_unique<AssociationInfo>();
				assoInfo->ip 	 = msg->peerIp();
				assoInfo->port 	 = msg->peerPort();
				assoInfo->stream = msg->peerStream();
				
				//AssociationInfo assInfo{msg->peerIp(), msg->peerPort(), msg->peerStream()};
				//association_list.insert(std::pair<unsigned int, AssociationInfo> (msg->getAssocId(), std::move(assInfo)))
				break;
			case SCTP_COMM_LOST:
				std::cout <<  "[Server]: Assoc change, COMMUNICATION LOST" << sctpAssociationChange->sac_assoc_id << std::endl;
				break;
			case SCTP_RESTART:
				std::cout <<  "[Server]: Assoc change, SCTP RESTART" << sctpAssociationChange->sac_assoc_id <<std::endl;
				break;
			case SCTP_SHUTDOWN_COMP:
				std::cout <<  "[Server]: Assoc change,SHUTDOWN COMPLETE" << sctpAssociationChange->sac_assoc_id <<std::endl;
				break;
			case SCTP_CANT_STR_ASSOC:
				std::cout <<  "[Server]: Assoc change,CAN'T START ASSOCIATION" << sctpAssociationChange->sac_assoc_id <<std::endl;
				break;
			default:
				std::cout << "[Server]: Assoc chagne with unknown type: 0x" << std::hex << sctpAssociationChange->sac_state << std::endl;
				break;
			}
			break;
		}
		default:
			std::cout << "[Server]: Other Notification: 0x" << std::hex << notification->sn_header.sn_type << std::endl;
			break;
	}

	//PrintAssocChange(notification);

	return 0;
}

int SctpServerEndpoint::onSctpMessages(std::unique_ptr<SctpMessageEnvelope> msg)
{
	std::cout << "[Server]: SCTP message received: " << *(msg->payloadData()) << std::endl;
		
	std::cout << "[Server]: Client address: " << *(msg->peerIp()) << std::endl;
	std::cout << "[Server]: Client port: " << msg->peerPort() << std::endl;
	
	std::cout << "[Server]: New association id = " << msg->associcationId() << std::endl;
	return 0;
}

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
	
	sctp_sendmsg(sock_op->socket_fd(), message.c_str(), rd_sz, 
				(sockaddr *) &clientAddr, sizeof(sockaddr_in), 
				0, 0, assoInfo->stream,    //SCTP_EOF
				0, 0);	
}
