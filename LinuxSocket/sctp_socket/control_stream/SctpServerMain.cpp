//SCTP Server
#include "common.h"
#include "SctpServerEndpoint.hpp"
#include <memory>
#include <iostream>

int main()
{
	int option;
	
	std::unique_ptr<SctpEndpoint> endpoint;  

	endpoint = std::make_unique<SctpServerEndpoint>("127.0.0.1", MY_PORT_NUM);
	
	endpoint->Start();

	return 0;
}