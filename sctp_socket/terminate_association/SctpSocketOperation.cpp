#include "SctpSocketOperation.hpp"


SctpSocketOperation::SctpSocketOperation()
{
	sock_fd = socket( AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP );
	
	if(sock_fd < 0){
		std::cout << "[Server]: Created Socket failed with errono: " << strerror(errno) << std::endl;
	}
}


SctpSocketOperation::~SctpSocketOperation()
{
	close(sock_fd);
}

void SctpSocketOperation::SetSocketOpt()
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
		std::cout << "[Server]: Error setsockopt(IPPROTO_SCTP): " << strerror(errno) << std::endl;
	}
	
	/* For client
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
      std::cout << "[Client]: error setsocketopt IPPROTO_SCTP: " << strerror(errno) << std::endl;
      return;
    }
	
	*/
}

void SctpSocketOperation::Bind(std::string localIp, uint32_t port)
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
		std::cout << "[Server]: sctp_bindx failed with errorno: " << strerror(errno) << std::endl;
	}
	
	if (-1 == listen( sock_fd, LISTEN_QUEUE))
	{
		std::cout << "[Server]: listen failed with errorno: " << strerror(errno) << std::endl;
	}
}


std::unique_ptr<SctpMessageEnvelope> SctpSocketOperation::Receive(int sock_fd)
{
	int msg_flags;
	size_t rd_sz;

	char readbuf[MAX_BUFFER+1];
	sockaddr_in cliaddr;
	sctp_sndrcvinfo sri;
	socklen_t len;
	
	len = sizeof(struct sockaddr_in);
	if(-1 == sctp_recvmsg(sock_fd, readbuf, sizeof(readbuf),
						 (struct sockaddr *)&cliaddr, &len, &sri, &msg_flags))
	{		
		std::cout << "[Poll Thread]: Error when sctp_recvmsg: " << strerror(errno) << std::endl;
		return nullptr;
	}
	
	std::unique_ptr<SctpMessageEnvelope> msg = std::make_unique<SctpMessageEnvelope>(readbuf, &cliaddr, &sri, msg_flags);
	
	return std::move(msg);
}
