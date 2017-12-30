#include <stdio.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>

// the max connection number of the server
#define MAX_CONNECTION_NUMBER 5

/* * Create a server endpoint of a connection. * Returns fd if all OK, <0 on error. */
int unix_socket_listen(const char *servername)
{ 
	int fd;
	struct sockaddr_un un; 
	
	/*int socket(int protofamily, int type, int protocol), return the socket discriptor
	 * protofamily
		-> AF_INET 	(IPV4)
		-> AF_INET6 (IPV6)
		-> AF_LOCAL/AF_UNIX  (Unix domian socket)
	 * type: type of the socket
		-> SOCK_STREAM: Provides sequenced, reliable, two-way, connection based byte streams
		-> SOCK_DGRAM: Supports datagrams (connectionless, unreliable messages of a fixed maximum length).
		-> SOCK_PACKET: Obsolete and should not be used in new programs
		-> SOCK_SEQPACKET
	 * protocol
		-> Normally only a single protocol exists to support a particular socket type within a given protocol family, 
		   in which case protocol can be specified as 0
	*/
	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
	{
		return(-1); 
	}
	
	printf("Evan, successful create socket!\n");
	
	int len, rval; 
	//unlink(servername);               /* in case it already exists */ 
	memset(&un, 0, sizeof(un)); 
	
	/* Initialization of socket address 
	 * sockaddr_un: address structure for unix socket
	 * ->  sa_family_t sun_family;               //AF_UNIX 
       ->  char        sun_path[108];            //pathname 
	 */
	un.sun_family = AF_UNIX; 
	strcpy(un.sun_path, servername); 
	

	len = offsetof(struct sockaddr_un, sun_path) + strlen(servername); 
		
	/*When a socket is created with socket(2), it exists in a name space but has no address assigned to it. 
	 *We need bind() to assign the address to the socket referred by the file discriptor. That is "assigning a name to a socket"
	 */
	
	
	/* bind the name to the descriptor 
	 * int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
	 * -> sockfd: socket descriptor
	 * -> addr: pointer to a structure, depends on the protocol family of the socket. if unix domain socket, the type shoudl be sockaddr_un.
	 * -> addrlen: the length of the address. 
	*/ 
	if (bind(fd, (struct sockaddr *)&un, len) < 0)
	{ 
		rval = -2; 
	} 
	else
	{
		printf("Evan, bind socket to addr %s! len = %d\n", un.sun_path, len);
		
		/* int listen(int sockfd, int backlog), 
		   marks the socket as a passive socket, that is, as a socket that will be used to accept incoming connection requests using accept(2)
		 * -> sockfd: file descriptor
		   -> backlog: defines the maximum length to which the queue of pending connections for sockfd may grow.  
		 **/
		
		if (listen(fd, MAX_CONNECTION_NUMBER) < 0)    
		{ 
			rval =  -3; 
		}
		else
		{
			printf("Evan, listen to the socket %d!\n", fd);

			return fd;
		}
	}
	
	/*clean if error happen*/
	int err;
	err = errno;
	close(fd); 
	errno = err;
	return rval;    
}

int unix_socket_accept(int listenfd, uid_t *uidptr)
{ 
	int clifd, len, rval; 
	time_t staletime; 
	struct sockaddr_un un;
	struct stat statbuf; 
	len = sizeof(un); 
	
	/* The accept() system call is used with connection-based socket types (SOCK_STREAM, SOCK_SEQPACKET)
	 *It extracts the first connection request on queue of pending connections for the listening socket. 
	 * int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen, int flags)
	 * -> sockfd: a socket is listening for conncetions, socket(2)->bind(2)->listen(2)
	 * -> addr: pointer to a sockaddr structure, filled with the addr of the peer socket. 
	 * */
	if ((clifd = accept(listenfd, (struct sockaddr *)&un, &len)) < 0) 
	{
		return(-1);     
	}
	
	printf("Evan, accept new connection clifd(%d), listenfd(%d)!\n", clifd, listenfd);

	
	/* obtain the client's uid from its calling address */ 
	len -= offsetof(struct sockaddr_un, sun_path);  /* len of pathname */
	un.sun_path[len] = 0; /* null terminate */ 
	if (stat(un.sun_path, &statbuf) < 0) 
	{
	  rval = -2;
	} 
	else
	{
		printf("Evan, un.sun_path after accept, %s, %d, %d!\n", un.sun_path, len, statbuf.st_uid);

		
	   if (S_ISSOCK(statbuf.st_mode) ) 
	   { 
		  if (uidptr != NULL) *uidptr = statbuf.st_uid;    /* return uid of caller */ 
		  //unlink(un.sun_path);       /* we're done with pathname now */ 
		  return clifd;         
	   } 
	   else
	   {
		  rval = -3;     /* not a socket */ 
	   }
	}
	int err;
	err = errno; 
	close(clifd); 
	errno = err;
	return(rval);
 }
 
 void unix_socket_close(int fd)
 {
    close(fd);     
 }

int main(void)
{ 
	int listenfd,connfd; 
	
	/*Listen to the socket*/
	listenfd = unix_socket_listen("foo.sock");
	if(listenfd<0)
	{
		printf("Error[%d] when listening...\n",errno);
		return 0;
	}
	printf("Finished listening...\n",errno);
	
	uid_t uid;
	connfd = unix_socket_accept(listenfd, &uid);
	unix_socket_close(listenfd);  
	if(connfd<0)
	{
		printf("Error[%d] when accepting...\n",errno);
		return 0;
	}
	
	printf("Begin to recv/send...\n");  
	int i,n,size;
	char rvbuf[2048];
	for(i=0;i<2;i++)
	{
		size = recv(connfd, rvbuf, 2047, 0);   
		if(size>=0)
		{
			// rvbuf[size]='\0';
			printf("Recieved Data[%d]:%c...%c\n",size,rvbuf[0],rvbuf[size-1]);
		}
		if(size==-1)
		{
			printf("Error[%d] when recieving Data:%s.\n",errno,strerror(errno));     
            break;        
		}
		sleep(10);
	}
	
	/*close socket*/
	unix_socket_close(connfd);
	printf("Server exited.\n");    
}