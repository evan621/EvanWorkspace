#include "common.h"
#include <unistd.h>
#include "SctpServerEndpoint.hpp"
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <poll.h>



SctpServerEndpoint::SctpServerEndpoint(std::string localIp, std::uint32_t port)
{
	std::cout << "[Server]: Create socket and bind to: " << localIp << ":" << port << std::endl;

	CreateSocket();
	SetSocketOpt();
	Bind(localIp, port);
}

SctpServerEndpoint::~SctpServerEndpoint()
{
	// close socket;
	close(sock_fd);

	std::cout << "[Server]: Close Socket!" << std::endl;
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

int SctpServerEndpoint::SctpMsgHandler(int sock_fd)
{
	int msg_flags;
	size_t rd_sz;

	char readbuf[MAX_BUFFER+1];
	sockaddr_in cliaddr;
	sctp_sndrcvinfo sri;
	socklen_t len;
	
	len = sizeof(struct sockaddr_in);
	rd_sz = sctp_recvmsg(sock_fd, readbuf, sizeof(readbuf),
						 (struct sockaddr *)&cliaddr, &len, &sri, &msg_flags);
	if(rd_sz == -1)
	{		
		std::cout << "[Server]: Error when sctp_recvmsg: " << strerror(errno) << std::endl;
		return -1;
	}
		
	if(msg_flags&MSG_NOTIFICATION) 
	{
		std::cout << "[Server]: Notification received" << std::endl;

		notification.Print(readbuf);
		return 0;
	}
	
	std::cout << "[Server]: SCTP message(size = " << rd_sz << " ) received: " << readbuf << std::endl;
	
	sctp_sendmsg(sock_fd, readbuf, rd_sz, 
				(sockaddr *) &cliaddr, len, 
				sri.sinfo_ppid, (sri.sinfo_flags), sri.sinfo_stream,    /*SCTP_EOF*/
				0, 0);
				
	std::cout << "[Server]: Send echo!" << std::endl;
		
	return 0;
}

int SctpServerEndpoint::StartPoolForMsg(int timeout)
{
	pollfd fdtable; 
	
	fdtable.fd = sock_fd;
	fdtable.events = POLLIN;
	
	std::cout << "[Server]: Waiting for new messages!" << std::endl;
	
	while(1)
	{
		switch(poll(&fdtable, 1, timeout)){
		case -1:
			std::cout << "[Server]: Error detected for poll: " << strerror(errno) << std::endl;
			break;
		case 0:
			std::cout << "[Server]: Time out for poll " << std::endl;
			return 0;
			break;
		default:
			if(fdtable.revents & POLLIN)
			{
				if(-1 == SctpMsgHandler(fdtable.fd))
				{
					return -1;
				}
			}
			break;
		}
	}
	
	return 0;
}

void SctpServerEndpoint::SendMsg()
{
}

void SctpServerEndpoint::RegisterMsgHandler()
{
	return;
}