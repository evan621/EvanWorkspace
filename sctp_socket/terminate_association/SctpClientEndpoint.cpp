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

	sock_op = std::make_unique<SctpSocketOperation>();
	sock_op->SetSocketOpt();
	
	// register fd
	io_multi = std::make_unique<IoMultiplex>();
	io_multi->RegisterFd(sock_op->socket_fd(), [this](int fd)
			{ SctpMsgHandler(fd); });
	io_multi->RegisterFd(STDIN, [this](int fd)
			{ ReadUserCmd(fd); });
	
	std::cout << "[Client]: Start Client, Register socket: " << sock_op->socket_fd() << std::endl;
}

SctpClientEndpoint::~SctpClientEndpoint()
{
	// close socket;
	std::cout << "[Client]: Close Client!" << std::endl;
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
		std::cout << "=============================================" << std::endl;
		std::cout << "[Client]: Waiting for new message or command!" << std::endl; 
		std::cout << "[Client]: [0] Exist the client!" << std::endl; 
		std::cout << "[Client]: [1] Send new message to server!" << std::endl; 

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
	//	return onSctpNotification(std::move(msg));
	}
	else
	{
		return onSctpMessages(std::move(msg));
	}
}


int SctpClientEndpoint::onSctpNotification(std::unique_ptr<SctpMessageEnvelope> msg)
{
	std::cout << "[Client]: Notification received" << std::endl;
	
	sctp_notification* notification;

	notification = (union sctp_notification *) msg->payloadData();
	
	switch(notification->sn_header.sn_type) {
		case SCTP_ASSOC_CHANGE: 
		{
			sctp_assoc_change *sctpAssociationChange;
			sctpAssociationChange = &notification->sn_assoc_change;
			switch(sctpAssociationChange->sac_state)
			{
			case SCTP_COMM_UP:
				std::cout << "[Client]: New association up with Id: " << msg->associcationId() << std::endl; 
				std::cout << "[Client]: New client IP: " << (*msg->peerIp()) << std::endl;
				std::cout << "[Client]: New client port: " << msg->peerPort() << std::endl;
				std::cout << "[Client]: New client trans stream: " << msg->peerStream() << std::endl;				
				//AssociationInfo assInfo{msg->peerIp(), msg->peerPort(), msg->peerStream()};
				//association_list.insert(std::pair<unsigned int, AssociationInfo> (msg->getAssocId(), std::move(assInfo)))
				break;
			case SCTP_COMM_LOST:
				std::cout <<  "[Client]: Assoc change, COMMUNICATION LOST" << msg->associcationId() << std::endl;
				break;
			case SCTP_RESTART:
				std::cout <<  "[Client]: Assoc change, SCTP RESTART" << msg->associcationId() <<std::endl;
				break;
			case SCTP_SHUTDOWN_COMP:
				std::cout <<  "[Client]: Assoc change,SHUTDOWN COMPLETE" << msg->associcationId() <<std::endl;
				break;
			case SCTP_CANT_STR_ASSOC:
				std::cout <<  "[Client]: Assoc change,CAN'T START ASSOCIATION" << msg->associcationId() <<std::endl;
				break;
			default:
				std::cout << "[Client]: Assoc change with unknown type: 0x" << std::hex << sctpAssociationChange->sac_state << std::endl;
				break;
			}
			break;
		}
		default:
			std::cout << "[Client]: Other Notification: 0x" << std::hex << notification->sn_header.sn_type << std::endl;
			break;
	}

	
	//notification.Print(msg->getPayload()->c_str());
	return 0;
}

int SctpClientEndpoint::onSctpMessages(std::unique_ptr<SctpMessageEnvelope> msg)
{
	std::cout << "[Client]: SCTP message received: " << (*msg->payloadData()) << std::endl;
		
	std::cout << "[sock add]: sa address: " << (*msg->peerIp()) << std::endl;
	std::cout << "[sock add]: sa port: " << msg->peerPort() << std::endl;
	
	std::cout << "[snd rcv info]: assoid = " << msg->associcationId() << std::endl;
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
	
	std::cout << "[Main thread]: Chose the message type to send: " <<std::endl;
	std::cout << "[Main thread]: [0] Abort Message" << std::endl;
	std::cout << "[Main thread]: [1] Normal message!" << std::endl;
	std::cout << "[Main thread]: [2] EOF" << std::endl;

	int option;
	int stream = 1;
	std::cin >> option;
	switch(option)
	{
		case 1:
		{
			std::cout << "[Main thread]: Input message content!" << std::endl;
			std::cin >> message ;

			sctp_sendmsg(sock_op->socket_fd(), message, 20, 
					(sockaddr *)&servaddr, sizeof(servaddr), 0, 0, stream, 0, 0);
			break;
		}
		case 0:
			sctp_sendmsg(sock_op->socket_fd(), message, 20, 
					(sockaddr *)&servaddr, sizeof(servaddr), 0, SCTP_ABORT, stream, 0, 0);
			std::cout << "[Main thread]: Send the abort to server!" << std::endl;
			break;
		case 2:
			sctp_sendmsg(sock_op->socket_fd(), message, 20, 
					(sockaddr *)&servaddr, sizeof(servaddr), 0, SCTP_EOF, stream, 0, 0);
			std::cout << "[Main thread]: Send the EOF to server!" << std::endl;
			break;
		default:
			std::cout << "[Main thread]: Wrong option!" << std::endl;
			break;
			
	}
}
