#include <stdio.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <string.h>

/* Create a client endpoint and connect to a server.   Returns fd if all OK, <0 on error. */
int unix_socket_conn(const char *servername)
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
 
 void unix_socket_close(int fd)
 {
    close(fd);     
 }


int main(void)
{ 
	srand((int)time(0));
  
	/*Create the socket and return the file discriptor*/
	int connfd; 
	connfd = unix_socket_conn("foo.sock");
	if(connfd<0)
	{
		printf("Error[%d] when connecting...",errno);
		return 0;
	}
  
  
	printf("Begin to recv/send...\n");  
	int i,n,size;
	char rvbuf[4096];
	for(i=0;i<10;i++)
	{
		memset(rvbuf,'a',2048);
		rvbuf[2047]='b';
		size = send(connfd, rvbuf, 2048, 0);
		if(size>=0)
		{
			printf("Data[%d] Sended:%c.\n",size,rvbuf[0]);
		}
		if(size==-1)
		{
			printf("Error[%d] when Sending Data:%s.\n",errno,strerror(errno));     
			break;        
		}
		sleep(5);
	}
	
	/*close the socket*/
	unix_socket_close(connfd);
	printf("Client exited.\n");    
 }

 