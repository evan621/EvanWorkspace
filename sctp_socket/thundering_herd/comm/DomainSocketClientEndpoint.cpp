#include "DomainSocketClientEndpoint.hpp"


DomainSocketClientEndpoint::DomainSocketClientEndpoint(const char *servername)
{
	conn_fd = domain_connect(servername);
	if(conn_fd<0)
	{
		printf("Error[%d] when connecting...",errno);
	}
}

DomainSocketClientEndpoint::~DomainSocketClientEndpoint()
{
	domain_close();
}

int DomainSocketClientEndpoint::domain_connect(const char *servername)
{ 
	int fd; 
	/*socket*/
	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)    /* create a UNIX domain stream socket */ 
	{
	return(-1);
	}
	printf("Evan, client creat new socket: %d!\n", fd);


	int len, rval;
	struct sockaddr_un un;          
	memset(&un, 0, sizeof(un));            /* fill socket address structure with our address */
	un.sun_family = AF_UNIX; 
	sprintf(un.sun_path, "scktmp%05d", getpid()); 
	len = offsetof(struct sockaddr_un, sun_path) + strlen(un.sun_path);
	//unlink(un.sun_path);               /* in case it already exists */ 

	printf("Begin to recv/send...\n");

	/*bind()*/
	if (bind(fd, (struct sockaddr *)&un, len) < 0)
	{ 
		rval=  -2; 
	} 
	else
	{
		printf("Evan, client bind to %s!\n", un.sun_path);

		/* fill socket address structure with server's address */
		memset(&un, 0, sizeof(un)); 
		un.sun_family = AF_UNIX; 
		strcpy(un.sun_path, servername); 
		len = offsetof(struct sockaddr_un, sun_path) + strlen(servername); 

		/*connect*/
		if (connect(fd, (struct sockaddr *)&un, len) < 0) 
		{
			rval= -4; 
		} 
		else
		{
			printf("Evan, client connect to %s!\n", un.sun_path);

			return (fd);
		}
	}
	int err;
	err = errno;
	close(fd); 
	errno = err;
	return rval;      
}

void DomainSocketClientEndpoint::domain_close()
{
	close(conn_fd);     
}

void DomainSocketClientEndpoint::send_msg()
{
	char rvbuf[2048];
	memset(rvbuf,'a',2048);
	rvbuf[2047]='b';
	int size = send(conn_fd, rvbuf, 2048, 0);
	
	if(size>=0)
	{
		printf("Data[%d] Sended:%c.\n",size,rvbuf[0]);
	}
}

