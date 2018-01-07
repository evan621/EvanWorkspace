#ifndef _SCTP_MESSAGE
#define _SCTP_MESSAGE

#include <memory>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <vector>

class SctpMessageEnvelope{
public:
	SctpMessageEnvelope(std::vector<char> readbuf, sockaddr_in* clientAddr, sctp_sndrcvinfo* sendRecvInfo, int msg_flags): 
		msg_flags(msg_flags),		
		payload(readbuf)
	{	
		if(clientAddr->sin_family != AF_INET)
		{
			std::cout << "[sock add]: Not a Internet addr" << std::endl;
		}
		
		char ip[20];
		inet_ntop(clientAddr->sin_family, &(clientAddr->sin_addr), ip, 20);
		
		ipaddr 	 = std::make_shared<std::string>(ip);
		port 	 = ntohs(clientAddr->sin_port);
		asso_id  = sendRecvInfo->sinfo_assoc_id;
		stream 	 = sendRecvInfo->sinfo_stream;
	}
	
	auto associcationId() { return asso_id; }
	auto peerIp() { return ipaddr; }
	auto peerPort() { return port; }
	auto peerStream() {return stream;}
	auto getPayload() { return payload; }
	auto payloadData() {return payload.data(); }
	void print(){
		unsigned int *a = (unsigned int*)payload.data();
		std::cout << "[print]: Received data size: " << std::hex << payload.size() << std::endl;
		std::cout << "buf[0]: " << std::hex << a[0] << ", " << std::endl;
		std::cout << "buf[1]: " << std::hex << a[1] << ", " << std::endl;
		std::cout << "buf[2]: " << std::hex << a[2] << ", " << std::endl;
		std::cout << "buf[3]: " << std::hex << a[3] << ", " << std::endl;	
	}
	auto flags() {return msg_flags;}
private:
	std::shared_ptr<std::string> ipaddr;
	in_port_t port;
	sctp_assoc_t asso_id;
	uint16_t stream;
	int msg_flags;

	std::vector<char> payload;
};

#endif