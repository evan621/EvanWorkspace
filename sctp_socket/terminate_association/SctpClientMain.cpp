#include "SctpClientEndpoint.hpp"
#include "common.h"
#include <iostream>
#include <memory>


int main()
{
	std::unique_ptr<SctpEndpoint> endpoint;  

	endpoint = std::make_unique<SctpClientEndpoint>();
	
	endpoint->SendMsg();	
}