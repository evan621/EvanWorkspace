#include "DomainSocket.hpp"
#include <errno.h>
#include <string.h>

DomainSocket::DomainSocket(std::string servername, int socketType, std::shared_ptr<spdlog::logger> logger): 
    sock_fd(-1), logger(logger)
{
    domain_create();

    switch(socketType)
    {
        case SERVER_DOMAIN_SOCKET:
            // bind and listen to loal
            domain_bind(servername);
            domain_listen();
            break;
        case CLIENT_DOMAIN_SOCKET:
        {
            // bind to a local address
            std::string localname = "./tmp/tmp" + std::to_string(getpid()) + ".sock";
            domain_bind(localname);
            domain_connect(servername);
            break;
        }
        default:
            logger->error("error socket type");
            break;
    }
}

DomainSocket::DomainSocket(int fd, std::shared_ptr<spdlog::logger> logger):sock_fd(fd), logger(logger)
{
}

DomainSocket::~DomainSocket()
{
    domain_close();
}

void DomainSocket::domain_create()
{
    sock_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    
    if (sock_fd < 0)
    {
        printf("[Err PID = %d]: Faled creating the domain socket with errno: %s ! Exit !\n", getpid(), strerror(errno));
        exit(-1); 
    }
}

void DomainSocket::domain_bind(std::string servername)
{
    struct sockaddr_un un; 
        
    int len, rval; 
    unlink(servername.c_str());
    memset(&un, 0, sizeof(un)); 
    
    // Init of sockaddr_un
    un.sun_family = AF_LOCAL; 
    strcpy(un.sun_path, servername.c_str()); 
    len = offsetof(struct sockaddr_un, sun_path) + servername.length(); 
    unlink(un.sun_path);
    
    // Bind
    if (bind(sock_fd, (struct sockaddr *)&un, len) < 0)
    { 
        printf("[Err PID = %d]: Faled bind the domain socket to addr %s with errno: %s ! Exit !\n", getpid(), servername.c_str(), strerror(errno));
        exit(-1); 
    } 
    
}

int DomainSocket::domain_listen()
{ 
    // Listen
    if(listen(sock_fd, MAX_CONNECTION_NUMBER) < 0)
    {
        printf("[Err PID = %d]: Faled listen to domain socket(fd = %d) with errno: %s ! Exit !\n", getpid(), sock_fd, strerror(errno));
        exit(-1);
    }
}

int DomainSocket::domain_accept()
{ 
    socklen_t len; 
    sockaddr_un un;
    len = sizeof(un); 
    int conn_fd;

    if ((conn_fd = accept(sock_fd, (sockaddr *)&un, &len)) < 0) 
    {
        printf("[Err PID = %d]: Faled accept to domain socket(fd = %d) with errno: %s ! Exit !\n", getpid(), sock_fd, strerror(errno));
        exit(-1);     
    }
    
    return conn_fd;
}

int DomainSocket::domain_connect(std::string servername)
{ 
    int rval;
    sockaddr_un un;

    /* fill socket address structure with server's address */
    memset(&un, 0, sizeof(un)); 
    un.sun_family = AF_UNIX; 
    strcpy(un.sun_path, servername.c_str()); 
    socklen_t len = offsetof(struct sockaddr_un, sun_path) + servername.length(); 

    /*connect*/
    if (connect(sock_fd, (struct sockaddr *)&un, len) < 0) 
    {
        logger->error("Domain Socket Client connect failed!");
        printf("[Err PID = %d]: Domain socket(fd = %d) Failed connect to (%s) with errno: %s ! Exit !\n", getpid(), sock_fd, servername.c_str(), strerror(errno));
        exit(-1);
    } 
    else
    {
        logger->info("Domain Socket Client connect to {} with fd {}!", servername, sock_fd);
        return (1);
    }
}

void DomainSocket::domain_close()
{
    if(sock_fd > 0)
    {
        close(sock_fd); 
        sock_fd = -1;
    }    
}

std::unique_ptr<SerializedMsg<internal_msg>> DomainSocket::domain_read()
{
    logger->info("Message recevied with fd {}", sock_fd);
    int size;
    char rvbuf[MAX_BUFFER];
    size = recv(sock_fd, rvbuf, MAX_BUFFER, 0);  

    std::unique_ptr<SerializedMsg<internal_msg>> recv_msg;

    switch(size)
    {
        case(0):
            //error, socket disconnected             
            printf("[Err PID = %d]: Domain socket(fd = %d) recv, connection shut down by peer !\n", getpid(), sock_fd);
            recv_msg = NULL;
            break;
        case(-1):
            // error when receiving the 
            printf("[Err PID = %d]: Domain socket(fd = %d)recv, error: %s occurse! EXIT!\n", getpid(), sock_fd, strerror(errno));
            exit(-1);
            break;
        default:
            // Data received
            recv_msg = std::make_unique<SerializedMsg<internal_msg>>(rvbuf, size);
            break;
    }

    return std::move(recv_msg);
}


void DomainSocket::domain_write(std::unique_ptr<SerializedMsg<internal_msg>> msg)
{    
    int size = send(sock_fd, (char *)msg->data(), msg->size(), 0);
    
    logger->info("domain_write data size = {}", size);

    if(size < 0)
    {
        printf("[Err PID = %d]: Domain socket(fd = %d) send failed with error: %s!\n", getpid(), sock_fd, strerror(errno));
    }
}

void DomainSocket::domain_send_msg(std::unique_ptr<SerializedMsg<internal_msg>> serial_msg)
{
    msghdr msg;

    // msg name
    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    //iov buffer set a dummy part
    iovec iov;

    iov.iov_base = serial_msg->data();
    iov.iov_len  = serial_msg->size();

    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_flags = 0;

    msg.msg_control = 0;
    msg.msg_controllen = 0;

    int ret = sendmsg(sock_fd, &msg, 0);
    
    if (ret == -1) {
        logger->error("sendmsg failed with {}", strerror(errno));
        printf("[Err PID = %d]: Domain socket(fd = %d) sendmsg failed with error: %s!\n", getpid(), sock_fd, strerror(errno));
        exit(-1);
    }
}

int DomainSocket::domain_send_fd(int fd)
{
    msghdr msg;
    
    // msg name
    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    //iov buffer set a dummy part
    char dummy = '$';
    iovec iov;

    iov.iov_base = &dummy;
    iov.iov_len = sizeof(dummy);

    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_flags = 0;

    //Set the fd to be transferred
    char cmsgbuf[CMSG_SPACE(sizeof(int))];
    
    msg.msg_control = cmsgbuf;
    msg.msg_controllen = CMSG_LEN(sizeof(int));

    cmsghdr* cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));

    *(int*) CMSG_DATA(cmsg) = fd;
        
    int ret = sendmsg(sock_fd, &msg, 0);

    if (ret == -1) {
        logger->error("sendmsg send fd failed with {}", strerror(errno));
        
        printf("[Err PID = %d]: Domain socket(fd = %d) sendmsg send fd(%d) failed with error: %s! EXIT!\n", getpid(), sock_fd, fd, strerror(errno));
        exit(-1);
    }

    return ret;
}

std::shared_ptr<SerializedMsg<internal_msg>> DomainSocket::domain_recv_msg(int& recvfd)
{
    msghdr msg;

    // set name
    msg.msg_name = 0;
    msg.msg_namelen = 0;

    // set the buffer to fill received msg
    char buf[MAX_BUFFER];
    iovec iov;
    iov.iov_base = buf;
    iov.iov_len = sizeof(buf);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    // msg control part
    cmsghdr *cmsg;
    char cms[CMSG_SPACE(sizeof(int))];

    msg.msg_flags = 0;
    msg.msg_control = (caddr_t) cms;
    msg.msg_controllen = sizeof(cms);

    switch(recvmsg(sock_fd, &msg, 0))
    {
        case 0:
            printf("[Err PID = %d]: Domain socket(fd = %d) recvmsg failed, peer shutdown connection!\n", getpid(), sock_fd);
            return nullptr;
            break;
        case -1:
            printf("[Err PID = %d]: Domain socket(fd = %d) recvmsg failed with error: %s!\n", getpid(), sock_fd, strerror(errno));
            exit(-1);
            break;
        default:
            break;
    }

    // Get the 1st control part
    cmsghdr *cmptr = CMSG_FIRSTHDR(&msg);

    if((cmptr != NULL) && (cmptr->cmsg_len == CMSG_LEN(sizeof(int))))
    {
        if((cmptr->cmsg_level == SOL_SOCKET) && (cmptr->cmsg_type == SCM_RIGHTS))
        {
            recvfd = *((int *)CMSG_DATA(cmptr));
            printf("[Info PID = %d]domain socket receive new fd: %d\n", getpid(), recvfd);
            return nullptr;
        }
    }

    auto serial_msg = std::make_shared<SerializedMsg<internal_msg>>((char *)msg.msg_iov[0].iov_base, msg.msg_iov[0].iov_len);
    return serial_msg;
}

