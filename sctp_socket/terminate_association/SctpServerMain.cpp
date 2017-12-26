//SCTP Server
#include "common.h"
#include "SctpServerEndpoint.hpp"
#include <memory>
#include <iostream>

int main()
{
	std::unique_ptr<SctpEndpoint> endpoint;  

	endpoint = std::make_unique<SctpServerEndpoint>("127.0.0.1", MY_PORT_NUM);
	
	std::cout << "end of main!" << std::endl;
	/*
	if (-1 == endpoint->StartPoolForMsg())
	{
		std::cout << "Polling failure" << std::endl;
	}*/
}