#include "SctpSocket.hpp"


SctpSocket::SctpSocket(std::string ip, uint32_t port, int socketType, std::shared_ptr<spdlog::logger> logger):
    logger(logger)
{
    // Socket works as server, one to one
    sctp_create();
    //set the socket option.
    sctp_opt_set();

    switch(socketType)
    {
        case SERVER_SCTP_SOCKET:
            // bind to a local ip
            bind_listen_to(ip, port);
            break;
        case CLIENT_SCTP_SOCKET:
            sctp_connect(ip, port);
            break;
        default:
            logger->error("error socket type");
            break;
    }
    
}

SctpSocket::SctpSocket(int fd, std::shared_ptr<spdlog::logger> logger):sock_fd(fd), logger(logger){}

SctpSocket::~SctpSocket()
{
    close(sock_fd);
}


int SctpSocket::sctp_accept()
{
    int len = sizeof(struct sockaddr_in);
    struct sockaddr_in client_addr;
    int conn_sockfd;

    conn_sockfd = accept(sock_fd, (struct sockaddr*)&client_addr, (socklen_t *)&len);
    
    logger->info("New SCTP Connection Established");
    
    return conn_sockfd;
}


void SctpSocket::sctp_create()
{
    sock_fd = socket( AF_INET, SOCK_STREAM, IPPROTO_SCTP );

    if(sock_fd < 0){
        logger->error("Created Socket failed with errono: {}", strerror(errno));

        printf("[Err PID = %d]: Faled creating the sctp socket with errno: %s ! Exit !\n", getpid(), strerror(errno));
        exit(-1);
    }
}


void SctpSocket::sctp_connect(std::string ip, uint32_t port)
{
    struct sockaddr_in servaddr;
    sctp_assoc_t  assoc_id;

    /* Specify the peer endpoint to which we'll connect */
    bzero( (void *)&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr( ip.c_str() );

    if(-1 != sctp_connectx(sock_fd, (struct sockaddr *)&servaddr, 1, &assoc_id))
    {
        logger->info("STCP Connection established, assoc id = {}", assoc_id);
        
        printf("[Err PID = %d]: Faled sctp_connectx sctp socket(fd = %d) to  with errno: %s ! Exit !\n", getpid(), ip.c_str(), strerror(errno));
        exit(-1);
        return;
    }
}



void SctpSocket::sctp_opt_set()
{
    struct sctp_event_subscribe evnts;  

    bzero(&evnts, sizeof(evnts));
    evnts.sctp_data_io_event = 1;
    evnts.sctp_association_event = 1;
    
#ifdef ENABLE_ALL_NOTIFICATION
    evnts.sctp_address_event = 1;
    evnts.sctp_send_failure_event = 1;
    evnts.sctp_peer_error_event = 1;
    evnts.sctp_shutdown_event = 1;
    evnts.sctp_partial_delivery_event = 1;
    evnts.sctp_adaptation_layer_event = 1;
    evnts.sctp_sender_dry_event = 1;
#endif
    
    if(-1 == setsockopt(sock_fd, IPPROTO_SCTP, SCTP_EVENTS, &evnts, sizeof(evnts)))
    {
        //std::cout << "[Server]: Error setsockopt(IPPROTO_SCTP): " << strerror(errno) << std::endl;
        logger->error("Error setsockopt(IPPROTO_SCTP): {}\n", strerror(errno));

        printf("[Err PID = %d]: Faled setsockopt for sctp socket(fd = %d) with errno: %s ! Exit !\n", getpid(), sock_fd, strerror(errno));
        exit(-1);
    }
}

void SctpSocket::bind_listen_to(std::string ip, uint32_t port)
{
    sockaddr_in servaddr;
    int addr_count = 1;

    /* Accept connections from any interface */
    bzero( (void *)&servaddr, sizeof(servaddr) );
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip.c_str());
    servaddr.sin_port = htons(port);

    // replace bind with sctp_bindx
    if(-1 == sctp_bindx( sock_fd, (struct sockaddr *)&servaddr, addr_count,  SCTP_BINDX_ADD_ADDR))
    {
        logger->error("sctp_bindx failed with errorno: {}", strerror(errno));
        printf("[Err PID = %d]: Faled setsockopt for sctp socket(fd = %d) with errno: %s ! Exit !\n", getpid(), sock_fd, strerror(errno));
        exit(-1);
    }
    
    if (-1 == listen( sock_fd, LISTEN_QUEUE))
    {
        logger->error("listen failed with errorno: {}", strerror(errno));
    }
}


std::unique_ptr<SctpMessageEnvelope> SctpSocket::sctp_read()
{
    int msg_flags;
    size_t rd_sz;

    sockaddr_in cliaddr;
    sctp_sndrcvinfo sri;
    socklen_t len;
    
    memset(&cliaddr, 0, sizeof(cliaddr));
    memset(&sri, 0, sizeof(sri));
        
    char readBuf[MAX_BUFFER+1];
    len = sizeof(struct sockaddr_in);
    if(-1 == sctp_recvmsg(sock_fd, readBuf, MAX_BUFFER,
                         (struct sockaddr *)&cliaddr, &len, &sri, &msg_flags))
    {       
        logger->error("Error when sctp_recvmsg: fd {}, error {}", sock_fd, strerror(errno));
        return nullptr;
    }

    std::vector<char> buf(readBuf, readBuf + sizeof(readBuf));
    std::unique_ptr<SctpMessageEnvelope> msg = std::make_unique<SctpMessageEnvelope>(buf, &cliaddr, &sri, msg_flags);
    
    return std::move(msg);
}

void SctpSocket::sctp_write(std::vector<char> msg)
{
    struct sctp_sndrcvinfo sinfo;
    
    bzero(&sinfo, sizeof(sinfo));
    sinfo.sinfo_stream = 1;
    sctp_send(sock_fd, msg.data(), msg.size(), &sinfo, 0);
}


