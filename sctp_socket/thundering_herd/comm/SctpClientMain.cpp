#include "SctpClientEndpoint.hpp"
#include "common.hpp"
#include <iostream>
#include <memory>


int main()
{
	std::unique_ptr<SctpEndpoint> endpoint;  

	endpoint = std::make_unique<SctpClientEndpoint>();
	
	endpoint->Start();
	
	return 0;
}