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
#include <stdio.h>


//#define ENABLE_ALL_NOTIFICATION

SctpClientEndpoint::SctpClientEndpoint(): continuepoll(true)
{

	sock_op = std::make_unique<SctpSocketOperation>();
	sock_op->SetSocketOpt();
	
	// register fd
	io_multi = std::make_unique<IoMultiplex>();
	io_multi->RegisterFd(sock_op->socket_fd(), [this](int fd)
			{ SctpMsgHandler(fd); });
	io_multi->RegisterFd(STDIN, [this](int fd)
			{ ReadUserCmd(fd); });
}

SctpClientEndpoint::~SctpClientEndpoint()
{
	// close socket;
	printf("[Client]: Close Client!\n");
}

void SctpClientEndpoint::ReadUserCmd(int fd)
{
	char buf;  
	read(fd, &buf, sizeof(buf));
	switch(buf)
	{
		case '0':
			std::cout << "[Client]: Exit server!" << std::endl; 
			continuepoll = false;
			break;
		case '1':
			SendMsg();
			break;
		default:
			std::cout << "[Client]: Unrecognized command!" << std::endl;
			break;
	}
}

void SctpClientEndpoint::Start()
{
	while(continuepoll)
	{
		printf("========================================================================\n");
		printf("[Client]: Waiting for new message or command.([0: Exit]; [1: NewMsg];)..\n");

		io_multi->Poll();
	}
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

				break;
			case SCTP_COMM_LOST:
				printf("[Server]: Assoc change(ID=%x), COMMUNICATION LOST\n", sctpAssociationChange->sac_assoc_id);
				break;
			case SCTP_RESTART:
				printf("[Server]: Assoc change(ID=%x), SCTP RESTART\n", sctpAssociationChange->sac_assoc_id);
				break;
			case SCTP_SHUTDOWN_COMP:
				printf("[Server]: Assoc change(ID=%x), SHUTDOWN COMPLETE\n", sctpAssociationChange->sac_assoc_id);
				break;
			case SCTP_CANT_STR_ASSOC:
				printf("[Server]: Assoc change(ID=%x), CAN'T START ASSOCIATION\n", sctpAssociationChange->sac_assoc_id);
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

	
	//notification.Print(msg->getPayload()->c_str());
	return 0;
}

int SctpClientEndpoint::onSctpMessages(std::unique_ptr<SctpMessageEnvelope> msg)
{
	printf("[Client]: SCTP message('') received from IP/Port(%s:%d) on assoc(0x%x) / stream(%d)\n ", 
			//*(msg->payloadData()),
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
	
	printf("[Client]: Chose message type, [0: Abort]; [1: Normal]; [2: EOF] ...\n");

	int option;
	int stream = 1;
	std::cin >> option;
	switch(option)
	{
		case 1:
		{
			printf("[Client]: Input message content!\n");
			std::cin >> message ;

			sctp_sendmsg(sock_op->socket_fd(), message, 20, 
					(sockaddr *)&servaddr, sizeof(servaddr), 0, 0, stream, 0, 0);
			break;
		}
		case 0:
			sctp_sendmsg(sock_op->socket_fd(), message, 20, 
					(sockaddr *)&servaddr, sizeof(servaddr), 0, SCTP_ABORT, stream, 0, 0);
			printf("[Client]: Send ABORT!\n");
			break;
		case 2:
			sctp_sendmsg(sock_op->socket_fd(), message, 20, 
					(sockaddr *)&servaddr, sizeof(servaddr), 0, SCTP_EOF, stream, 0, 0);
			printf("[Client]: Send EOF!\n");
			break;
		default:
			printf("[Client]: Wrong Option!\n");
			break;
			
	}
}
