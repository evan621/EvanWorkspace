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
		payload = std::make_unique<std::string>(readbuf);
		
		if(clientAddr->sin_family != AF_INET)
		{
			std::cout << "[sock add]: Not a Internet addr" << std::endl;
		}
		
		char ip[20];
		inet_ntop(clientAddr->sin_family, &(clientAddr->sin_addr), ip, 20);
		
		ipaddr 	 = std::make_unique<std::string>(ip);
		port = clientAddr->sin_port;
		
		asso_id = sendRecvInfo->sinfo_assoc_id;
		
	}
	
	auto getAssocId() { return asso_id; }
	auto peerIp() { return ipaddr->c_str(); }
	auto peerPort() { return port; }
	auto getPayload() { return payload->c_str(); }
	
private:
	std::unique_ptr<std::string> ipaddr;
	int port;
	std::unique_ptr<std::string> payload;
	int asso_id;
};

#endif