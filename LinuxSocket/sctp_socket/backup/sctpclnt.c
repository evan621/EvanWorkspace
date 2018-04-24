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
#include <string.h>
#include <poll.h>


#define ENABLE_ALL_NOTIFICATION

void poll_msg(int sock_fd)
{
	int rd_sz;
	int msg_flags;
	struct pollfd fds;
	struct sockaddr_in peeraddr;
	socklen_t len;
	char recvline[MAX_BUFFER + 1];
	struct sctp_sndrcvinfo sri;
		
	while(1){
		bzero(&sri, sizeof(sri));

		fds.fd = sock_fd;
		fds.events = POLLIN;
	
		switch(poll(&fds, 1, 100)){
			case -1:
				printf("Error detected for poll: %s\n", strerror(errno));
				break;
			case 0:
				return;
			default:
				//printf("fds::revents: %x\n", fds.revents);
				if(fds.revents & POLLIN)
				{
					len = sizeof(peeraddr);
					rd_sz = sctp_recvmsg(sock_fd, recvline, sizeof(recvline),
								&peeraddr, &len, &sri, &msg_flags);
					
					if(msg_flags & MSG_NOTIFICATION)
					{
						printf("[Client]: Receive the notification!\n");
						
						PrintSctpNotification(recvline);
					}
					else
					{
						printf("[Server]: readsize:%d, stream: %d, seq: %d, association: 0x%x: \n", 
								rd_sz, sri.sinfo_stream, sri.sinfo_ssn, (u_int)sri.sinfo_assoc_id);
					
						printf("%.*s\n", rd_sz, recvline);	
					}
				}
			break;
		}
	}
}


void sctpstr_cli(FILE *fp, int sock_fd, struct sockaddr *to, socklen_t tolen)
{
	char sendline[MAX_BUFFER + 1];
	struct sctp_sndrcvinfo sri;
	int out_sz;
	
	bzero(&sri, sizeof(sri));
	
	printf("[Client]: Input a text to be sent:\n");
	
	while(fgets (sendline, MAX_BUFFER, fp) != NULL){
		if(sendline[0] == 'E'){
			return;
		}
		
		if(sendline[0] != '[') {
			printf("Error, line must be '[stream num] text' format or 'E' to exit!\n");
			continue;
		}
		
		sri.sinfo_stream = strtol(&sendline[1], NULL, 0);
		out_sz = strlen(sendline);
		
		sctp_sendmsg(sock_fd, sendline, out_sz, 
					to, tolen, 0, 0, sri.sinfo_stream, 0, 0);
		
		// polling for echo
		poll_msg(sock_fd);
	}
	
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
	
#ifdef ENABLE_ALL_NOTIFICATION
	events.sctp_address_event = 1;
	events.sctp_send_failure_event = 1;
	events.sctp_peer_error_event = 1;
	events.sctp_shutdown_event = 1;
	events.sctp_partial_delivery_event = 1;
	events.sctp_adaptation_layer_event = 1;
	events.sctp_sender_dry_event = 1;
#endif
	if(0 != setsockopt( sock_fd, SOL_SCTP, SCTP_EVENTS,
					 (const void *)&events, sizeof(events) ))
	{
	  printf("[Client]: error setsocketopt SOL_SCTP, %s\n", strerror(errno));
	  return;
	}
}

void GetSocketOpt(int sock_fd)
{
	int in;
	struct sctp_status status;

	/* Read and emit the status of the Socket (optional step) */
	in = sizeof(status);
	getsockopt( sock_fd, SOL_SCTP, SCTP_STATUS, (void *)&status, (socklen_t *)&in );

	printf("assoc id  = %d\n", status.sstat_assoc_id );
	printf("state     = %d\n", status.sstat_state );
	printf("instrms   = %d\n", status.sstat_instrms );
	printf("outstrms  = %d\n", status.sstat_outstrms );
}

int main()
{
	int connSock;
	struct sockaddr_in servaddr;

	/* Create an SCTP TCP-Style Socket */
	connSock = socket( AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP );
	SetSocketOpt(connSock);
	
	/* Specify the peer endpoint to which we'll connect */
	bzero( (void *)&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(MY_PORT_NUM);
	servaddr.sin_addr.s_addr = inet_addr( "127.0.0.1" );
	sctpstr_cli(stdin, connSock, &servaddr, sizeof(servaddr));
	
	printf("[Client]: Close socket!\n");

	/* Close our socket and exit */
	close(connSock);

	return 0;
}

