#include "common.h"
#include <unistd.h>
#include "SctpServerEndpoint.hpp"
#include <iostream>
#include <errno.h>
#include <string.h>
#include <poll.h>


SctpServerEndpoint::SctpServerEndpoint(std::string localIp, std::uint32_t port): continuepoll(true)
{
	std::cout << "[Server]: Create socket and bind to: " << localIp << ":" << port << std::endl;

	CreateSocket();
	SetSocketOpt();
	Bind(localIp, port);
	
	// create new thread for the polling of received messages
	pollThread = std::thread(&SctpServerEndpoint::StartPoolForMsg, this);
}

SctpServerEndpoint::~SctpServerEndpoint()
{	
	std::cout << "[Server]: SctpServerEndpoint destructor called!" << std::endl;
	
	continuepoll = false;

	pollThread.join();
	close(sock_fd);
}

void SctpServerEndpoint::CreateSocket()
{
	sock_fd = socket( AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP );
	
	if(sock_fd < 0){
		std::cout << "[Server]: Created Socket failed with errono: " << strerror(errno) << std::endl;
	}
}

void SctpServerEndpoint::Bind(std::string localIp, uint32_t port)
{
	sockaddr_in servaddr;
    int addr_count = 1;

    /* Accept connections from any interface */
    bzero( (void *)&servaddr, sizeof(servaddr) );
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(localIp.c_str());
    servaddr.sin_port = htons(port);

    // replace bind with sctp_bindx
    if(-1 == sctp_bindx( sock_fd, (struct sockaddr *)&servaddr, addr_count,  SCTP_BINDX_ADD_ADDR))
	{
		std::cout << "[Server]: sctp_bindx failed with errorno: " << strerror(errno) << std::endl;
	}
	
	if (-1 == listen( sock_fd, LISTEN_QUEUE))
	{
		std::cout << "[Server]: listen failed with errorno: " << strerror(errno) << std::endl;
	}
}

void SctpServerEndpoint::SetSocketOpt()
{
	struct sctp_event_subscribe evnts;  

	bzero(&evnts, sizeof(evnts));
	evnts.sctp_data_io_event = 1;
	evnts.sctp_association_event = 1;
	
#ifdef ENABLE_ALL_NOTIFICATION
	evnts.sctp_address_event = 1;
	evnts.sctp_send_failure_event = 1;
	evnts.sctp_peer_error_event = 1;
	evnts.sctp_shutdown_event = 1;
	evnts.sctp_partial_delivery_event = 1;
	evnts.sctp_adaptation_layer_event = 1;
	evnts.sctp_sender_dry_event = 1;
#endif
	
	if(-1 == setsockopt(sock_fd, IPPROTO_SCTP, SCTP_EVENTS, &evnts, sizeof(evnts)))
	{
		std::cout << "[Server]: Error setsockopt(IPPROTO_SCTP): " << strerror(errno) << std::endl;
	}
}

int SctpServerEndpoint::handleSctpNotification(std::unque_ptr<SctpMessageEnvelope> msg)
{
	std::cout << "[Poll Thread]: Notification received" << std::endl;

	notification.Print(msg->getPayload());
	return 0;
}

int SctpServerEndpoint::handleSctpMessage(std::unque_ptr<SctpMessageEnvelope> msg)
{
	std::cout << "[Poll Thread]: SCTP message received: " << msg->getPayload() << std::endl;
		
	std::cout << "[sock add]: sa address: " << msg->peerIp() << std::endl;
	std::cout << "[sock add]: sa port: " << msg->peerPort() << std::endl;
	
	std::cout << "[snd rcv info]: assoid = " << msg->getAssocId() << std::endl;
	return 0;
}

int SctpServerEndpoint::SctpMsgHandler(int sock_fd)
{
	int msg_flags;
	size_t rd_sz;

	char readbuf[MAX_BUFFER+1];
	sockaddr_in cliaddr;
	sctp_sndrcvinfo sri;
	socklen_t len;
	
	len = sizeof(struct sockaddr_in);
	if(-1 == sctp_recvmsg(sock_fd, readbuf, sizeof(readbuf),
						 (struct sockaddr *)&cliaddr, &len, &sri, &msg_flags))
	{		
		std::cout << "[Poll Thread]: Error when sctp_recvmsg: " << strerror(errno) << std::endl;
		return -1;
	}
	
	std::unque_ptr<SctpMessageEnvelope> msg(readbuf, &cliaddr, &sri);
		
	if(msg_flags&MSG_NOTIFICATION) 
	{
		return handleSctpNotification(msg);
	}
	else
	{
		return handleSctpMessage(msg);
	}
	
		
	return 0;
}

int SctpServerEndpoint::StartPoolForMsg()
{
	pollfd fdtable; 
	
	fdtable.fd = sock_fd;
	fdtable.events = POLLIN;
	
	std::cout << "[Poll Thread]: Waiting for new messages!" << std::endl;
	
	while(continuepoll)
	{
		switch(poll(&fdtable, 1, 1000)){
		case -1:
			std::cout << "[Poll Thread]: Error detected for poll: " << strerror(errno) << std::endl;
			break;
		case 0:
			//std::cout << "[Poll Thread]: Time out for poll " << std::endl;
			//return 0;
			break;
		default:
			if(fdtable.revents & POLLIN)
			{
				if(-1 == SctpMsgHandler(fdtable.fd))
				{
					return -1;
				}
			}
			else
			{
				std::cout << "[Poll Thread]: Exception during poll, revents = " << std::hex << fdtable.revents << std::endl;
			}
			break;
		}
	}

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

void SctpServerEndpoint::RegisterMsgHandler()
{
	return;
}