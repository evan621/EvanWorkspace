#include "DomainSocketServerEndpoint.hpp"


DomainSocketServerEndpoint::DomainSocketServerEndpoint(const char *servername, IoMultiplex& multiRecv): io_multi(multiRecv), listen_fd(0), conn_fd(0), isEndPointReady(false)
{
	domain_listen(servername);
	
	io_multi.RegisterFd(listen_fd, [this](int sock_fd) 
						{ 	
							if(sock_fd != listen_fd)
							{
								printf("wrong fd!");
								return;
							}
							domain_accept();
						} );	
}

DomainSocketServerEndpoint::~DomainSocketServerEndpoint()
{
	domain_close();
}

void DomainSocketServerEndpoint::ready()
{
	while(!isEndPointReady)
	{
		io_multi.Poll();
	}
}


int DomainSocketServerEndpoint::domain_listen(const char *servername)
{ 
	struct sockaddr_un un; 

	if ((listen_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
	{
		return(-1); 
	}
		
	int len, rval; 
	//unlink(servername);
	memset(&un, 0, sizeof(un)); 
	
	// Init of sockaddr_un
	un.sun_family = AF_UNIX; 
	strcpy(un.sun_path, servername); 
	len = offsetof(struct sockaddr_un, sun_path) + strlen(servername); 
	
	// Bind
	if (bind(listen_fd, (struct sockaddr *)&un, len) < 0)
	{ 
		rval = -2; 
	} 
	else
	{
		printf("Domain Socket Server Created! Bind to: %s\n", un.sun_path);

		// Listen
		if (listen(listen_fd, MAX_CONNECTION_NUMBER) < 0)    
		{ 
			rval =  -3; 
		}
		else
		{
			printf("Domain Socket Server Listen to: %d!\n", listen_fd);
			return 0;
		}
	}
	
	/*clean if error happen*/
	int err;
	err = errno;
	close(listen_fd); 
	listen_fd = 0;
	errno = err;
	return rval;    
}

int DomainSocketServerEndpoint::domain_accept()
{ 
	socklen_t len; 
	sockaddr_un un;
	len = sizeof(un); 

	if ((conn_fd = accept(listen_fd, (sockaddr *)&un, &len)) < 0) 
	{
		return -1;     
	}

	io_multi.RegisterFd(conn_fd, [this](int sock_fd) 
						{ 	
							if(conn_fd != listen_fd)
							{
								printf("wrong conn fd!");
								return;
							}
							domain_receive();
						} );	

	isEndPointReady = true;					
	
	printf("Domain Socket Server Accept new client: conn_fd(%d), listen_fd(%d)!\n", conn_fd, listen_fd);
	return 0;
}

void DomainSocketServerEndpoint::domain_close()
{
	if(listen_fd > 0)
	{
		close(listen_fd); 
		listen_fd = 0;
	}    
	
	if(conn_fd > 0)
	{
		close(conn_fd); 
		conn_fd = 0;
	}    
}

void DomainSocketServerEndpoint::domain_receive()
{
	int size;
	char rvbuf[2048];
	size = recv(conn_fd, rvbuf, 2047, 0);   
	if(size>=0)
	{
		// rvbuf[size]='\0';
		printf("Domain Socket Server Recieved Data[%d]:%c...%c\n",size,rvbuf[0],rvbuf[size-1]);
	}
	else if(size==-1)
	{
		printf("Error[%d] when recieving Data:%s.\n",errno,strerror(errno));     
	}

}


