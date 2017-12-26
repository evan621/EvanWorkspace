//SCTP Server

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include "common.h"
#include <errno.h>
#include <string.h>
#include "sctpnotification.h"


int Socket()
{
    /* Create SCTP TCP-Style Socket */
    return socket( AF_INET, SOCK_STREAM, IPPROTO_SCTP );
}

void Bind(int sock_fd)
{
    struct sockaddr_in servaddr;
    int addr_count = 1;

    /* Accept connections from any interface */
    bzero( (void *)&servaddr, sizeof(servaddr) );
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl( INADDR_ANY );
    servaddr.sin_port = htons(MY_PORT_NUM);

    // replace bind with sctp_bindx
    if(-1 == sctp_bindx( sock_fd, (struct sockaddr *)&servaddr, addr_count,  SCTP_BINDX_ADD_ADDR))
    {
        printf("bind socket failed: %s \n", strerror(errno));
        return;
    }
}

         
int SetSocketOpt(int sock_fd)
{
    struct sctp_event_subscribe evnts;  

    bzero(&evnts, sizeof(evnts));
    evnts.sctp_data_io_event = 1;

    if(0 != setsockopt(sock_fd, IPPROTO_SCTP, SCTP_EVENTS, &evnts, sizeof(evnts)))
    {
        printf("Error setsockopt(IPPROTO_SCTP): %s\n", strerror(errno));
        return 0;
    }

    return 1;
}

void Listen(int sock_fd)
{
    int listenQueLen = 5;
    listen( sock_fd, listenQueLen);
}

int Accept(int list_sockfd)
{
    int len = sizeof(struct sockaddr_in);
    struct sockaddr_in client_addr;
    int conn_sockfd;

    printf("[Server]: Awaiting a new connection\n");
    conn_sockfd = accept(list_sockfd, (struct sockaddr*)&client_addr, &len);
    
    printf("[Server]: Connection Established\n");
    
    return conn_sockfd;
}

void SctpRecevMsg(int sock_fd)
{
    int msg_flags;
    size_t rd_sz;

    char readbuf[MAX_BUFFER+1];
    struct sockaddr_in cliaddr;
    struct sctp_sndrcvinfo sri;
    socklen_t len;
    
    len = sizeof(struct sockaddr_in);
    
    while(1)
    {
        int rz = sctp_recvmsg(sock_fd, readbuf, sizeof(readbuf),
                              &cliaddr, &len, &sri, &msg_flags);
        
        if (rz == 0)
        {
            printf("[Server]: EOF received!\n");
            return;
        }
        else if(rz < 0)
        {
            printf("[Server]: Error sctp_recvmsg: %s\n", strerror(errno));
            return;
        }
        
        printf("[Server]: Message(size = %d): '%s' received \n", rz , readbuf);
    }
}

void Close(int sock_fd)
{
    close(sock_fd);
    
    printf("[Server]: Close connect socket!\n");
}

int main()
{
   // int list_sockfd, conn_sockfd;
	
	struct sockaddr_in sa;
	char ip[20];
	inet_pton(AF_INET, "172.0.0.1", &(sa.sin_addr));
	printf("[inet_pton]: IP addr --> binary: %x\n", sa.sin_addr);

	inet_ntop(AF_INET, &(sa.sin_addr), ip, 20);
	printf("[inet_ntop]: binary --> IP address: %s\n", ip);
	
	printf("[inet_addr]: IP addr --> binary: %x\n", inet_addr(ip));

  
  /*
    list_sockfd = Socket();
    Bind(list_sockfd);
    SetSocketOpt(list_sockfd);
    Listen(list_sockfd);
    
    while( 1 ) {
        int conn_sockfd = Accept(list_sockfd);
        
        SctpRecevMsg(conn_sockfd);
        
        Close(conn_sockfd);
    }
*/
    return 0;
}