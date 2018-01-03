#include "SctpClientEndpoint.hpp"
#include "common.h"
#include <iostream>
#include <memory>


int main()
{
	std::unique_ptr<SctpEndpoint> endpoint;  

	std::cout << "[Main thread]: Input your choice: " <<std::endl;
	std::cout << "[Main thread]: [0] Exit the client!" << std::endl;
	std::cout << "[Main thread]: [1] Send message to server" << std::endl;

	endpoint = std::make_unique<SctpClientEndpoint>();
	int option;
	while(1)
	{
		std::cin >> option;
		switch(option)
		{
		case 0:
			return 0;
			break;
		case 1:
			endpoint->SendMsg();
			break;
		default:
			break;
		}
		
	}
}