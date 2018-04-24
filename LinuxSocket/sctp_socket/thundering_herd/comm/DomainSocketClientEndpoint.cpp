#include "DomainSocketClientEndpoint.hpp"


DomainSocketClientEndpoint::DomainSocketClientEndpoint(const char *servername, std::shared_ptr<IoMultiplex> multiRecv, std::shared_ptr<spdlog::logger> logger):
    logger(logger), io_multi(multiRecv)
{
    logger->info("DomainSocketClientEndpoint construct");

    conn_fd = domain_connect(servername);
    if(conn_fd<0)
    {
        logger->info("Error[%d] when connecting...",errno);
        exit(0);
    }

    io_multi->register_fd(conn_fd, [this](int sock_fd) 
                        {   
                            domain_receive(sock_fd);
                        } ); 
}

void DomainSocketClientEndpoint::domain_receive(int sock_fd)
{
    logger->info("Server Domain Socket, Message recevied with fd {}", sock_fd);
    int size;
    char rvbuf[MAX_BUFFER];
    size = recv(sock_fd, rvbuf, MAX_BUFFER, 0);   

    if(size>0)
    {
        // rvbuf[size]='\0';
        logger->info("Server Domain Socket Recieved Data[{}]{}...{}\n",size,rvbuf[0],rvbuf[size-1]);
    }
    else if(size==-1)
    {
        logger->info("Error[%d] when recieving Data:{}.\n",errno,strerror(errno));     
    }
    else
    {
        logger->error("message size is 0");
        exit(0);
    }

    
    std::vector<char> recv_msg(rvbuf, rvbuf + size);

    msg_handler(recv_msg);
}


DomainSocketClientEndpoint::~DomainSocketClientEndpoint()
{
    domain_close();
}

int DomainSocketClientEndpoint::domain_connect(const char *servername)
{ 
    int fd; 
    /*socket*/
    if ((fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0)    /* create a UNIX domain stream socket */ 
    {
        logger->error("Domain Socket Client create failed!");
        return(-1);
    }
    logger->info("Domain Socket Client create success with fd({})!", fd);


    int len, rval;
    struct sockaddr_un un;          
    memset(&un, 0, sizeof(un));            /* fill socket address structure with our address */
    un.sun_family = AF_LOCAL; 
    sprintf(un.sun_path, "./tmp/tmp%05d.sock", getpid()); 
    len = offsetof(struct sockaddr_un, sun_path) + strlen(un.sun_path);
    unlink(un.sun_path);               /* in case it already exists */ 

    //bind
    if (bind(fd, (struct sockaddr *)&un, len) < 0)
    { 
        logger->error("Domain Socket Client bind failed!");
        rval=  -2; 
    } 
    else
    {
        logger->info("Domain Socket Client bind to {}!", un.sun_path);

        /* fill socket address structure with server's address */
        memset(&un, 0, sizeof(un)); 
        un.sun_family = AF_UNIX; 
        strcpy(un.sun_path, servername); 
        len = offsetof(struct sockaddr_un, sun_path) + strlen(servername); 

        /*connect*/
        if (connect(fd, (struct sockaddr *)&un, len) < 0) 
        {
            logger->error("Domain Socket Client connect failed!");
            rval= -4; 
        } 
        else
        {
            logger->info("Domain Socket Client connect to {}!", un.sun_path);

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
void DomainSocketClientEndpoint::send_msg(std::vector<char> msg)
{    
    int size = send(conn_fd, (char *)msg.data(), msg.size(), 0);

    if (size < 0)
    {
        logger->error("Msg send to domain socket({}} failed!\n", conn_fd);
    }
}

