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
	
	while(1)
	{
		std::cout << "Input your choice: " <<std::endl;
		std::cout << "[0] Exit the server!" << std::endl;
		std::cout << "[1] Send message to client" << std::endl;
		std::cin >> option;
		switch(option)
		{
		case 0:
			std::cout << "exit the server" << std::endl;
			return 0;
			break;
		case 1:
			std::cout << "Send message to client" << std::endl;
			endpoint->SendMsg();
			break;
		default:
			break;
		}
	}
	
	return 0;
}