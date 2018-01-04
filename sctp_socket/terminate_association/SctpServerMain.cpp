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
	/*
	std::cout << "[Main thread]: Input your choice at any time you wish: " <<std::endl;
	std::cout << "[Main thread]: [0] Exit the server!" << std::endl;
	std::cout << "[Main thread]: [1] Send message to client" << std::endl;

	while(1)
	{
		std::cin >> option;
		switch(option)
		{
		case 0:
			std::cout << "[Main thread]: exit the server" << std::endl;
			return 0;
			break;
		case 1:
			std::cout << "[Main thread]: Send message to client" << std::endl;
			endpoint->SendMsg();
			break;
		default:
			break;
		}
	}
	*/
	return 0;
}