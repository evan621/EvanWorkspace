#ifndef _SCTP_MESSAGE
#define _SCTP_MESSAGE

#include <memory>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

class SctpMessageEnvelope{
public:
	SctpMessageEnvelope(char* readbuf, sockaddr_in* clientAddr, sctp_sndrcvinfo* sendRecvInfo)
	{
		payload = std::make_shared<std::string>(readbuf);
		
		if(clientAddr->sin_family != AF_INET)
		{
			std::cout << "[sock add]: Not a Internet addr" << std::endl;
		}
		
		char ip[20];
		inet_ntop(clientAddr->sin_family, &(clientAddr->sin_addr), ip, 20);
		
		ipaddr 	 = std::make_shared<std::string>(ip);
		port = ntohs(clientAddr->sin_port);
		
		asso_id = sendRecvInfo->sinfo_assoc_id;
		
		stream = sendRecvInfo->sinfo_stream;
	}
	
	auto associcationId() { return asso_id; }
	auto peerIp() { return ipaddr; }
	auto peerPort() { return port; }
	auto peerStream() {return stream;}
	auto getPayload() { return payload; }
	
private:
	std::shared_ptr<std::string> ipaddr;
	in_port_t port;
	std::shared_ptr<std::string> payload;
	sctp_assoc_t asso_id;
	uint16_t stream;
};

#endif