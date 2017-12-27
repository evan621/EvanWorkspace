class SctpMessageEnvelope{
	SctpMessageEnvelope(char* readbuf, sockaddr_in* clientAddr, sctp_sndrcvinfo* sendRecvInfo)
	{
		payload = std::make_unique<std::string>(readbuf);
		
		if(sa->sin_family != AF_INET)
		{
			std::cout << "[sock add]: Not a Internet addr" << std::endl;
		}
		
		char ip[20];
		inet_ntop(clientAddr->sin_family, &(clientAddr->sin_addr), ip, 20);
		
		ip 	 = std::make_unique<std::string>(ip);
		port = clientAddr->sin_port;
		
		asso_id = sendRecvInfo->sinfo_assoc_id;
		
	}
	
	auto getAssocId() { return asso_id; }
	auto peerIp() { return ip->c_str(); }
	auto peerPort() { return port; }
	auto getPayload() { return payload->c_str(); }
	
public
	std::uniqueptr<std::string> ip;
	auto port;
	std::uniqueptr<std::string> payload;
	auto asso_id;
};
