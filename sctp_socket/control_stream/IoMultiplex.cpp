#include "IoMultiplex.hpp"


void IoMultiplex::RegisterFd(int fd, CallBack cb)
{
	pollfd item;
	item.fd = fd;
	item.events = POLLIN;
	poll_items.push_back(std::move(item));
	
	cb_lists.push_back(std::move(cb));
}

void IoMultiplex::PollEventHandler()
{
	for(auto i = 0; i < poll_items.size(); i++)
	{
		if(poll_items.at(i).revents & POLLIN)
		{
			cb_lists.at(i)(poll_items.at(i).fd);
			return;
		}
	}
}

void IoMultiplex::Poll()
{
	switch(poll(poll_items.data(), poll_items.size(), -1))
	{
		case -1:
			std::cout << "Error while polling!" << std::endl;
			break;
		case 0:
			//timeout 
			break;
		default:
			PollEventHandler();
			// new message
			break;
	}
}