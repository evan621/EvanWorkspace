#include "SctpServerEndpoint.hpp"
#include "common.h"
#include <iostream>
#include <errno.h>
#include <string.h>
#include <stdio.h>


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
			
	printf("[Server]: Server Start and bind to addr(%s:%d)\n", localIp.c_str(), port);
}

SctpServerEndpoint::~SctpServerEndpoint()
{	
	std::cout << "[Server]: SctpServerEndpoint destructor called!" << std::endl;
}

void SctpServerEndpoint::Start()
{	
	while(continuepoll)
	{
		printf("====================================================================\n");
		printf("[Server]: Wait for new message or command([0: Exit]; [1: NewMessage])...\n");
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
	printf("[Server]: Notification received!\n");
	
	sctp_notification* notification = (sctp_notification*)msg->payloadData();
			
	switch(notification->sn_header.sn_type) {
		case SCTP_ASSOC_CHANGE: 
		{
			sctp_assoc_change *sctpAssociationChange;
			sctpAssociationChange = &notification->sn_assoc_change;
			switch(sctpAssociationChange->sac_state)
			{
			case SCTP_COMM_UP:
				printf("[Server]: Assoc change, COMMUNICATION UP! ClientAddr{ IP/Port(%s:%d) }\n", msg->peerIp()->c_str(),  msg->peerPort());
				
				assoInfo = std::make_unique<AssociationInfo>();
				assoInfo->ip 	 = msg->peerIp();
				assoInfo->port 	 = msg->peerPort();
				
				//AssociationInfo assInfo{msg->peerIp(), msg->peerPort(), msg->peerStream()};
				//association_list.insert(std::pair<unsigned int, AssociationInfo> (msg->getAssocId(), std::move(assInfo)))
				break;
			case SCTP_COMM_LOST:
				printf("[Server]: Assoc change(ID=0x%x), COMMUNICATION LOST\n", sctpAssociationChange->sac_assoc_id);
				break;
			case SCTP_RESTART:
				printf("[Server]: Assoc change(ID=0x%x), SCTP RESTART\n", sctpAssociationChange->sac_assoc_id);
				break;
			case SCTP_SHUTDOWN_COMP:
				printf("[Server]: Assoc change(ID=0x%x), SHUTDOWN COMPLETE\n", sctpAssociationChange->sac_assoc_id);
				break;
			case SCTP_CANT_STR_ASSOC:
				printf("[Server]: Assoc change(ID=0x%x), CAN'T START ASSOCIATION\n", sctpAssociationChange->sac_assoc_id);
				break;
			default:
				printf("[Server]: Assoc chagne with unknown type (0x%x)\n", sctpAssociationChange->sac_state);
				break;
			}
			break;
		}
		default:
			printf("[Server]: Other Notification: (0x%x)\n", notification->sn_header.sn_type);
			break;
	}

	//PrintAssocChange(notification);

	return 0;
}

int SctpServerEndpoint::onSctpMessages(std::unique_ptr<SctpMessageEnvelope> msg)
{
	assoInfo->stream = msg->peerStream();
	printf("[Server]: SCTP message('') received from IP/Port(%s:%d) on assoc(0x%x) / stream(%d)\n ", 
			//*message,
			msg->peerIp()->c_str(),
			msg->peerPort(),
			msg->associcationId(),
			msg->peerStream());
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
