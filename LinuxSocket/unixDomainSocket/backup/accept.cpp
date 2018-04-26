#include <stddef.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>


int serv_accept(int listenfd, uid_t *uidptr)
{
	int		clifd, len, err, rval;
	time_t	staletime;
	struct  sockaddr_un un;
	struct	stat		statbud;
	
	len = sizeof(un);
	if ((clifd = accept(listenfd, (struct sockaddr * )&un, &len)) < 0)
	{
		return(-1);
	}
	
	len -= offsetof(struct sockaddr_un, sun_path); 
	un.sun_path[len] = 0;
	
	if(stat(un.sun_path, &statbuf) < 0)
	{
		rval = -2;
		goto errout;
	}
	if(S_ISSOCK(statbuf.st_mode) == 0) 
	{
		rval = -3;
		goto errout;
	}
	
	if (uidptr != NULL) 
	{
		*uidptr = statbuf.st_uid;
	}
	
	unlink(un.sun_path);
	return(clifd);

errout:
	err = errno;
	close(clifd);
	errno = err;
	return(rval);
}