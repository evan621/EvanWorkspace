#include <errno.h>
#include <string.h>

#include <iostream>
#include <sys/types.h>        /* See NOTES */
#include <sys/socket.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <unistd.h>

#define LOGD(...) do { printf(__VA_ARGS__); printf("\n"); } while(0)
#define LOGE(...) do { printf(__VA_ARGS__); printf("\n"); } while(0)
#define LOGW(...) do { printf(__VA_ARGS__); printf("\n"); } while(0)


using namespace std;

/**
 * Sends given file descriptior via given socket
 *
 * @param socket to be used for fd sending
 * @param fd to be sent
 * @return sendmsg result
 *
 * @note socket should be (PF_UNIX, SOCK_DGRAM)
 */
int sendfd(int socket, int fd) {
    char dummy[1] = {1};
    struct msghdr msg;
    struct iovec iov;

    char cmsgbuf[CMSG_SPACE(sizeof(int))];

    iov.iov_base = &dummy;
    iov.iov_len = sizeof(dummy);

    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_flags = 0;
    msg.msg_control = cmsgbuf;
    msg.msg_controllen = CMSG_LEN(sizeof(int));

    struct cmsghdr* cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));

    *(int*) CMSG_DATA(cmsg) = fd;

    int ret = sendmsg(socket, &msg, 0);

	printf("sendmsg, size, %d\n", ret);
	
    if (ret == -1) {
        LOGE("sendmsg failed with %s", strerror(errno));
    }

    return ret;
}

/**
 * Receives file descriptor using given socket
 *
 * @param socket to be used for fd recepion
 * @return received file descriptor; -1 if failed
 *
 * @note socket should be (PF_UNIX, SOCK_DGRAM)
 */
int recvfd(int socket) {
    int len;
    int fd;
    char buf[5];
    struct iovec iov;
    struct msghdr msg;
    struct cmsghdr *cmsg;
    //char cms[CMSG_SPACE(sizeof(int))];
	char cms[CMSG_SPACE(10)];
	
    iov.iov_base = buf;
    iov.iov_len = sizeof(buf);

    msg.msg_name = 0;
    msg.msg_namelen = 0;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_flags = 0;
    msg.msg_control = (caddr_t) cms;
    msg.msg_controllen = sizeof cms;

    len = recvmsg(socket, &msg, 0);

//	printf("recvmsg, controllen, %d\n", msg.msg_controllen);

	printf("recvmsg, len: %d, iov: %d, (%d, %d, %d, %d, %d)\n", len, iov.iov_len, 
				buf[0], buf[1], buf[2], buf[3], buf[4]);

	printf("recvmsg, controllen, %d, %d\n", msg.msg_controllen, sizeof(cms));

    if (len < 0) {
        LOGE("recvmsg failed with %s", strerror(errno));
        return -1;
    }

    if (len == 0) {
        LOGE("recvmsg failed no data");
        return -1;
    }

    cmsg = CMSG_FIRSTHDR(&msg);
    memmove(&fd, CMSG_DATA(cmsg), sizeof(int));
		
    return fd;
}

#define TXTSRV "server\n"
#define TXTCLI "client\n"

void main_server(int socket) {
    int fd;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    char filename[] = "file";
    fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, mode);

    write(fd, TXTSRV, strlen(TXTSRV));
    sendfd(socket, fd);
    close(fd);
}

void main_client(int socket) {
    int fd = recvfd(socket);
    write(fd, TXTCLI, strlen(TXTCLI));
    close(fd);
}

int main() {
    int pid;
    int pair[2];

    cout << "started..." << endl;

    if (socketpair(AF_LOCAL, SOCK_STREAM, 0, pair) < 0) {
        cout << "socketpair failed" << endl;
        return 1;
    }

    if ((pid = fork()) < 0) {
        cout << "fork failed" << endl;
        return 1;
    }

    if (pid != 0) {
        cout << "i am a parent" << endl;
        close(pair[0]);
        main_server(pair[1]);
    } else {
        cout << "i am a child" << endl;
        close(pair[1]);
        main_client(pair[0]);
    }

    return 0;
}