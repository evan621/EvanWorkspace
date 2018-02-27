#include "SctpSocket.hpp"


SctpSocket::SctpSocket(std::string localIp, uint32_t port, int socketType)
{
    // Socket works as server, one to one
    // create socket
    create();
    //set the socket option.
    setSocketOpt();

    switch(socketType)
    {
        case SERVER_SCTP_SOCKET:
            // bind to a local ip
            bindAndListenTo(localIp, port);
            break;
        case CLIENT_SCTP_SOCKET:
            connect(localIp, port);
            break;
        default:
            printf("error socket type");
            break;
    }
    
}

SctpSocket::SctpSocket(int fd):sock_fd(fd)
{
    // Create a SctpSocket with a given fd
}


SctpSocket::~SctpSocket()
{
    close(sock_fd);
}


int SctpSocket::accept()
{
    int len = sizeof(struct sockaddr_in);
    struct sockaddr_in client_addr;
    int conn_sockfd;

    printf("[Server]: Awaiting a new connection\n");
    conn_sockfd = accept(sock_fd, (struct sockaddr*)&client_addr, &len);
    
    printf("[Server]: Connection Established\n");
    
    return conn_sockfd;
}


int SctpSocket::create()
{
    sock_fd = socket( AF_INET, SOCK_STREAM, IPPROTO_SCTP );

    if(sock_fd < 0){
        //std::cout <<  << strerror(errno) << std::endl;
        printf("[Server]: Created Socket failed with errono: %s\n", strerror(errno));
        exit(0);
    }
}


void SctpSocket::connect(std::string localIp, uint32_t port)
{
    struct sockaddr_in servaddr;
    sctp_assoc_t  assoc_id;

    /* Specify the peer endpoint to which we'll connect */
    bzero( (void *)&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr( localIp.c_str() );

    if(-1 != sctp_connectx(sock_fd, (struct sockaddr *)&servaddr, 1, &assoc_id))
    {
        printf("[Client]: Connection established, id = %x\n", assoc_id);
        return;
    }
}



void SctpSocket::setSocketOpt()
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
        printf("Error setsockopt(IPPROTO_SCTP): %s\n", strerror(errno));
    }
    
    sctp_initmsg initmsg;
    sctp_event_subscribe evnts;  
    
    // Specify that a maximum of 5 streams will be available per socket 
    memset( &initmsg, 0, sizeof(initmsg) );
    initmsg.sinit_num_ostreams = 5;
    initmsg.sinit_max_instreams = 5;
    initmsg.sinit_max_attempts = 4;
    if(0 != setsockopt( sock_fd, IPPROTO_SCTP, SCTP_INITMSG,
                     &initmsg, sizeof(initmsg) ))
    {
      std::cout << "error setsocketopt IPPROTO_SCTP: " << strerror(errno) << std::endl;
      return;
    }
    
}

void SctpSocket::bindAndListenTo(std::string localIp, uint32_t port)
{
    sockaddr_in servaddr;
    int addr_count = 1;

    /* Accept connections from any interface */
    bzero( (void *)&servaddr, sizeof(servaddr) );
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(localIp.c_str());
    servaddr.sin_port = htons(port);

    // replace bind with sctp_bindx
    if(-1 == sctp_bindx( sock_fd, (struct sockaddr *)&servaddr, addr_count,  SCTP_BINDX_ADD_ADDR))
    {
        printf("sctp_bindx failed with errorno: %s\n", strerror(errno));

    }
    
    if (-1 == listen( sock_fd, LISTEN_QUEUE))
    {
        printf("listen failed with errorno: %s\n", strerror(errno));
    }
}


std::unique_ptr<SctpMessageEnvelope> SctpSocket::read()
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
        printf("[Server]: Error when sctp_recvmsg: %s\n", strerror(errno));
        return nullptr;
    }
        
    std::vector<char> buf(readBuf, readBuf + sizeof(readBuf));
    std::unique_ptr<SctpMessageEnvelope> msg = std::make_unique<SctpMessageEnvelope>(buf, &cliaddr, &sri, msg_flags);
    
    return std::move(msg);
}

void SctpSocket::write(std::vector<char> msg)
{
    struct sctp_sndrcvinfo sinfo;
    
    bzero(&sinfo, sizeof(sinfo));
    sinfo.sinfo_stream = 1;
    sctp_send(sock_fd, msg.data(), msg.size(), &sinfo, 0);
}


