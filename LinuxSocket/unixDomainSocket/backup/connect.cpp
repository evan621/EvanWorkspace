#include <stdio.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <errno.h>


#define CLI_PATH "/var/path"

int cli_conn(const char *name)
{
	int 	fd, len, err, rval;
	struct  sockaddr_un un;
	
	if((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
	{
		return(-1);
	}
	
	memset(&un, 0, sizeof(un));
	un.sun_family = AF_UNIX;
	sprintf(un.sun_path, "%s%05d", CLI_PATH, getpid());
	
	len = offsetof(struct sockaddr_un)
}