#ifndef _SCTP_MESSAGE
#define _SCTP_MESSAGE

#include <memory>
#include <stdio.h>
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
			printf("[sock add]: Not a Internet addr!\n");
		}
		
		char ip[20];
		inet_ntop(clientAddr->sin_family, &(clientAddr->sin_addr), ip, 20);
		
		ipaddr 	 = std::make_shared<std::string>(ip);
		port 	 = ntohs(clientAddr->sin_port);
		asso_id  = sendRecvInfo->sinfo_assoc_id;
		stream 	 = sendRecvInfo->sinfo_stream;
		ssn		 = sendRecvInfo->sinfo_ssn;
	}
	
	auto associcationId() { return asso_id; }
	auto peerIp() { return ipaddr; }
	auto peerPort() { return port; }
	auto peerStream() {return stream;}
	auto msgSsn() { return ssn; }
	auto getPayload() { return payload; }
	auto payloadData() {return payload.data(); }
	void print(){
		unsigned int *a = (unsigned int*)payload.data();
		printf("[print]: Received data size: %d\n", (unsigned int)payload.size());
		printf("buf[0]: %x\n", a[0]);
		printf("buf[1]: %x\n", a[1]);
		printf("buf[2]: %x\n", a[2]);
		printf("buf[3]: %x\n", a[3]);
	}
	auto flags() {return msg_flags;}
private:
	std::shared_ptr<std::string> ipaddr;
	in_port_t port;
	sctp_assoc_t asso_id;
	uint16_t stream;
	uint16_t ssn;
	int msg_flags;

	std::vector<char> payload;
};

#endif