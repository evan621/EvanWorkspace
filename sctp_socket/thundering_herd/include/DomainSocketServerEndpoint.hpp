#ifndef _DOMAIN_SOCKET_SERVER_ENDPOINT
#define _DOMAIN_SOCKET_SERVER_ENDPOINT

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include "IoMultiplex.hpp"

#define MAX_CONNECTION_NUMBER 10

class DomainSocketServerEndpoint
{
public:
	DomainSocketServerEndpoint(const char *servername, IoMultiplex& multiRecv);
	~DomainSocketServerEndpoint();
	
	void ready();
private:
	int listen_fd;
	int conn_fd;
	IoMultiplex& io_multi;
	bool isEndPointReady;
	
	int domain_listen(const char *servername);
	int domain_accept();
	void domain_close();
	void domain_receive();

};


#endif
