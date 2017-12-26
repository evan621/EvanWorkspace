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


//#define ENABLE_ALL_NOTIFICATION

SctpClientEndpoint::SctpClientEndpoint()
{
	std::cout << "[Client]: Create new socket: SEQPACKET " << std::endl;

	CreateSocket();
	SetSocketOpt();
}

SctpClientEndpoint::~SctpClientEndpoint()
{
	// close socket;
	close(sock_fd);

	std::cout << "[Client]: Close Socket!" << std::endl;
}

void SctpClientEndpoint::SetSocketOpt()
{
	sctp_initmsg initmsg;
	sctp_event_subscribe evnts;  
	
    /* Specify that a maximum of 5 streams will be available per socket */
    memset( &initmsg, 0, sizeof(initmsg) );
    initmsg.sinit_num_ostreams = 5;
    initmsg.sinit_max_instreams = 5;
    initmsg.sinit_max_attempts = 4;
    if(0 != setsockopt( sock_fd, IPPROTO_SCTP, SCTP_INITMSG,
                     &initmsg, sizeof(initmsg) ))
    {
      std::cout << "[Client]: error setsocketopt IPPROTO_SCTP: " << strerror(errno) << std::endl;
      return;
    }
	
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
	
	if(0 != setsockopt(sock_fd, IPPROTO_SCTP, SCTP_EVENTS, &evnts, sizeof(evnts)))
	{
		std::cout << "[Client]: Error setsockopt(IPPROTO_SCTP): " << strerror(errno) << std::endl;
	}
}


void SctpClientEndpoint::CreateSocket()
{
	sock_fd = socket( AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP );
	
	if(sock_fd < 0){
		std::cout << "[Client]: Created Socket failed with errono: " << strerror(errno) << std::endl;
	}
}

int SctpClientEndpoint::SctpMsgHandler(int sock_fd)
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
		std::cout << "[Client]: Error when sctp_recvmsg: " << strerror(errno) << std::endl;
		return -1;
	}
		
	if(msg_flags&MSG_NOTIFICATION) 
	{
		std::cout << "[Client]: Notification received" << std::endl;

		notification.Print(readbuf);
		return 0;
	}
	
	std::cout << "[Client]: SCTP message(size = " << rd_sz << " ) received: " << readbuf << std::endl;
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
	sctp_sendmsg(sock_fd, message, 20, 
				(sockaddr *)&servaddr, sizeof(servaddr), 0, 0, stream, 0, 0);
				
	usleep(3000000);
	
	std::cout << "Send abort" << std::endl;
	
	sctp_sendmsg(sock_fd, message, 20, 
				(sockaddr *)&servaddr, sizeof(servaddr), 0, SCTP_ABORT, stream, 0, 0);
				
	std::cout << "wait for 3 s" << std::endl;

	usleep(3000000);
}

void SctpClientEndpoint::SendAbort()
{

}

void SctpClientEndpoint::RegisterMsgHandler()
{
	return;
}

int SctpClientEndpoint::StartPoolForMsg()
{
	pollfd fdtable; 
	
	fdtable.fd = sock_fd;
	fdtable.events = POLLIN;
	
	std::cout << "[Client]: Waiting for new messages!" << std::endl;
	
	while(1)
	{
		switch(poll(&fdtable, 1, TIME_OUT)){
		case -1:
			std::cout << "[Client]: Error detected for poll: " << strerror(errno) << std::endl;
			break;
		case 0:
			std::cout << "[Client]: Time out for poll " << std::endl;
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
