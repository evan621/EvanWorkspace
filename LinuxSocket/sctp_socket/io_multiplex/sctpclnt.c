//SCTP client

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
#include "common.h"
#include <errno.h>

int Socket()
{
	return socket( AF_INET, SOCK_STREAM, IPPROTO_SCTP );
}

void SetSocketOpt(int sock_fd)
{
    struct sctp_initmsg initmsg;
    struct sctp_event_subscribe events;
    int opt = 1;
    int len = sizeof(opt);

    /* Specify that a maximum of 5 streams will be available per socket */
    memset( &initmsg, 0, sizeof(initmsg) );
    initmsg.sinit_num_ostreams = 5;
    initmsg.sinit_max_instreams = 5;
    initmsg.sinit_max_attempts = 4;
    if(0 != setsockopt( sock_fd, IPPROTO_SCTP, SCTP_INITMSG,
                     &initmsg, sizeof(initmsg) ))
    {
      printf("[Client]: error setsocketopt IPPROTO_SCTP, %s\n", strerror(errno));
      return;
    }

    /* Enable receipt of SCTP Snd/Rcv Data via sctp_recvmsg */
    memset( (void *)&events, 0, sizeof(events) );
    events.sctp_data_io_event = 1;
    events.sctp_association_event = 1;
    
    if(0 != setsockopt( sock_fd, SOL_SCTP, SCTP_EVENTS,
                     (const void *)&events, sizeof(events) ))
    {
      printf("[Client]: error setsocketopt SOL_SCTP, %s\n", strerror(errno));
      return;
    }
}


void Connect(int sock_fd)
{
	struct sockaddr_in servaddr;
    sctp_assoc_t  assoc_id;

    /* Specify the peer endpoint to which we'll connect */
    bzero( (void *)&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MY_PORT_NUM);
    servaddr.sin_addr.s_addr = inet_addr( "127.0.0.1" );

    if(-1 != sctp_connectx(sock_fd, (struct sockaddr *)&servaddr, 1, &assoc_id))
	{
		printf("[Client]: Connection established, id = %x\n", assoc_id);
		return;
	}
}

void SendMsg(int sock_fd)
{
	char buf[20];
	struct sctp_sndrcvinfo sinfo;
	
	printf("[Client]: Input message to send to server!\n");
	if(NULL != gets(buf))
	{
		bzero(&sinfo, sizeof(sinfo));
		sinfo.sinfo_stream = 1;
		sctp_send(sock_fd, buf, strlen(buf), &sinfo, 0);
	}
}

void Close(int sock_fd)
{
	char buf[20];
	printf("[Client]: Enter to close socket!\n");
	if(NULL != gets(buf))
	{
		/* Close our socket and exit */
		close(sock_fd);
		printf("[Client]: Close socket!\n");

		return;
	}
}

int main()
{
    int sock_fd;
    /* Create an SCTP TCP-Style Socket */
    sock_fd = Socket();
    SetSocketOpt(sock_fd);
	Connect(sock_fd);
    
	SendMsg(sock_fd);
	Close(sock_fd);
    return 0;
}