#include "DomainSocketServerEndpoint.hpp"


DomainSocketServerEndpoint::DomainSocketServerEndpoint(const char *servername, std::shared_ptr<IoMultiplex> multiRecv, std::shared_ptr<spdlog::logger> logger): 
    io_multi(multiRecv), listen_fd(0), logger(logger)
{
    logger->info("DomainSocketServerEndpoint construct");

    domain_listen(servername);
    
    io_multi->register_fd(listen_fd, [this](int sock_fd) 
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

int DomainSocketServerEndpoint::domain_listen(const char *servername)
{ 
    struct sockaddr_un un; 

    if ((listen_fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0)
    {
        return(-1); 
    }
        
    int len, rval; 
    unlink(servername);
    memset(&un, 0, sizeof(un)); 
    
    // Init of sockaddr_un
    un.sun_family = AF_LOCAL; 
    strcpy(un.sun_path, servername); 
    len = offsetof(struct sockaddr_un, sun_path) + strlen(servername); 
    
    // Bind
    if (bind(listen_fd, (struct sockaddr *)&un, len) < 0)
    { 
        rval = -2; 
    } 
    else
    {
        logger->info("Server Domain Socket Created! Bind to: {}", un.sun_path);

        // Listen
        if (listen(listen_fd, MAX_CONNECTION_NUMBER) < 0)    
        { 
            rval =  -3; 
        }
        else
        {
            logger->info("Server Domain Socket Listen to: {}", listen_fd);
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
    struct stat statbuf; 
    len = sizeof(un); 
    int conn_fd;

    if ((conn_fd = accept(listen_fd, (sockaddr *)&un, &len)) < 0) 
    {
        return -1;     
    }

    /* obtain the client's uid from its calling address */ 
    len -= offsetof(struct sockaddr_un, sun_path);  /* len of pathname */
    un.sun_path[len] = 0; /* null terminate */ 
    stat(un.sun_path, &statbuf);

    logger->info("un.sun_path after accept, {}, {}, {}!\n", un.sun_path, len, statbuf.st_uid);

    io_multi->register_fd(conn_fd, [this](int sock_fd) 
                        {   
                            domain_receive(sock_fd);
                        } );  

    conn_client_fds.push_back(conn_fd);                    

    logger->info("Server Domain Socket Accept new client: conn_fd({}), client({}, {}, {})!\n", conn_fd, un.sun_path, len, statbuf.st_uid);
    
    return 0;
}

void DomainSocketServerEndpoint::domain_close()
{
    if(listen_fd > 0)
    {
        close(listen_fd); 
        listen_fd = 0;
    }    

    std::vector<int>::iterator it; 
    for(it = conn_client_fds.begin(); it != conn_client_fds.end(); ++it)
    {
        close(*it); 
    }    
}

void DomainSocketServerEndpoint::domain_receive(int sock_fd)
{
    logger->info("Message recevied with fd {}", sock_fd);
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
        logger->error("message size is 0, the connection to fd({}) lost", sock_fd);
        io_multi->deregister_fd(sock_fd);
        return;
    }

    std::vector<char> recv_msg(rvbuf, rvbuf + size);

    msg_handler(recv_msg);
}


void DomainSocketServerEndpoint::publish_msg(std::vector<char> msg)
{    
    std::vector<int>::iterator it; 

    for(it = conn_client_fds.begin(); it != conn_client_fds.end(); ++it)
    {
        int size = send(*it, (char *)msg.data(), msg.size(), 0);
        printf("Server Msg send(%d), size(%d)\n", getpid(), size);

        if (size > 0)
        {
            logger->info("Data[{}] Sended:{} to fd {}.\n",size, msg.data()[0], *it);
        }
    }    
}

