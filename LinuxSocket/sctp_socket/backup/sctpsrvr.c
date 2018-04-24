#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include "common.h"
#include <poll.h>
#include <errno.h>
#include <string.h>
#include "sctpnotification.h"

#define DEBUG_LOG
#define ENABLE_ALL_NOTIFICATION

int Socket()
{
	return socket( AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP );
}

void PrintSockAddr(struct sockaddr_in* sa)
{
	char ip[20];

	if(sa->sin_family != AF_INET)
	{
		printf("[sock add]: Not a Internet addr");
	}
	inet_ntop(sa->sin_family, &(sa->sin_addr), ip, 20);
	printf("[sock add]: sa address: %s\n", ip);
	printf("[sock add]: sa port: %d\n", ntohs(sa->sin_port));
}

void PrintSndRcvInfo(struct sctp_sndrcvinfo *sri)
{
	printf("[snd rcv info]: assoid(%x), ppid(%x), flags(%x), stream(%x)\n", sri->sinfo_assoc_id, sri->sinfo_ppid, sri->sinfo_flags, sri->sinfo_stream);
}


int SctpMsgProcess(int sock_fd)
{
	int msg_flags;
	size_t rd_sz;

	char readbuf[MAX_BUFFER+1];
	struct sockaddr_in cliaddr;
	struct sctp_sndrcvinfo sri;
	socklen_t len;
	
	len = sizeof(struct sockaddr_in);
	rd_sz = sctp_recvmsg(sock_fd, readbuf, sizeof(readbuf),
						  &cliaddr, &len, &sri, &msg_flags);	
	
	if(rd_sz != -1)
	{
		printf("[Server]: SCTP message(flag = %x) received with size: %d start! \n", msg_flags, rd_sz);
		
		if(msg_flags&MSG_NOTIFICATION) 
		{
			printf("[Server]: Notification received \n");
			
			PrintSctpNotification(readbuf);
			return 1;
		}
#ifdef DEBUG_LOG
		PrintSockAddr(&cliaddr);
		PrintSndRcvInfo(&sri);
#endif
		int ret = sctp_sendmsg(sock_fd, readbuf, rd_sz, 
					 &cliaddr, len, 
					 sri.sinfo_ppid, 
					 sri.sinfo_flags, sri.sinfo_stream, 0, 0);
				 
		if(-1 == ret)
		 {
			 printf("[Server-err]: send echo failed: %s!\n", strerror(errno));
			 return 0;
		 }
		 
		 if(readbuf[4] == 'C')
		 {
			 printf("[Server-err]: Cancel received!\n");
			 return 0;
		 }
		 
		 printf("[Server]: SCTP message receive end!\n");
	} else
	{
		printf("[Server]: Error when sctp_recvmsg: %s !\n", strerror(errno));
	}
	return 1;
}

int SetSocketOpt(int sock_fd)
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
	
	if(0 != setsockopt(sock_fd, IPPROTO_SCTP, SCTP_EVENTS, &evnts, sizeof(evnts)))
	{
		printf("Error setsockopt(IPPROTO_SCTP): %s\n", strerror(errno));
		return 0;
	}

	return 1;
}

void Bind(int sock_fd)
{
	struct sockaddr_in servaddr;

	/* Accept connections from any interface */
	bzero( (void *)&servaddr, sizeof(servaddr) );
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl( INADDR_ANY );
	servaddr.sin_port = htons(MY_PORT_NUM);

	if(0 != bind( sock_fd, (struct sockaddr *)&servaddr, sizeof(servaddr) ))
	{
		printf("bind socket failed: %s \n", strerror(errno));
		return;
	}
}

void Listen(int sock_fd)
{
	int listenQueLen = 5;
	listen( sock_fd, listenQueLen );
}

int main()
{
	int sock_fd;
  
	/* Create SCTP TCP-Style Socket */
	sock_fd = Socket();
	Bind(sock_fd);
	SetSocketOpt(sock_fd);
	Listen(sock_fd);
	/* Place the server socket into the listening state */
	
	printf("[Server]: Awaiting a new connection\n");

	/* Server loop... */
	while( 1 ) {
		struct pollfd fds;
		
		fds.fd = sock_fd;
		fds.events = POLLIN;
		
		switch(poll(&fds, 1, -1)){
			case -1:
				printf("Error detected for poll: %s\n", strerror(errno));
				break;
			case 0:
				printf("Time out for poll\n");
				break;
			default:
				if(fds.revents & POLLIN)
				{
					if(!SctpMsgProcess(fds.fd))
					{
						printf("[Server]: Close socket!\n");
						close(sock_fd);
						return 0;
					}
				}
				break;
		}
	}

	return 0;
}

