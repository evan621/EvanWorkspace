#include "zmsg.hpp"
#include <iostream>
#include <map>

class broker
{
public:
	broker()
	{
		m_context = new zmq::context_t(1);
		m_socket = new zmq::socket_t(*m_context, ZMQ_ROUTER);
	}

	~broker()
	{

	}

	void bind(std::string endpoint)
	{
		m_endpoint = endpoint;
		std::cout << "[mdbrk]: connect to broker" << std::endl;
		
		m_socket->bind(m_endpoint.c_str());
	}

	void start_brocking(){
		std::cout << "[mdbrk]: start brocking" << std::endl;
		
		while(1) {
			//{socket, fd, events, revents}
			// fd: file discriptor
			// events: ZMQ_POLLIN/ZMQ_POLLOUT/ZMQ_POLLERR
			zmq::pollitem_t items [] = 
				{*m_socket, 0, ZMQ_POLLIN, 0};  
						
			//(*items, nitems, timeout), timeout = 0 means return immediately,  timeout = 1 means indefinitely
			zmq::poll(items, 1, -1);
						
			if(items[0].revents & ZMQ_POLLIN){
				zmsg* recv_msg = new zmsg(*m_socket);
				
				recv_msg->dump();
			}
		}
	}

private:
	zmq::context_t* m_context;
	zmq::socket_t* m_socket;

	std::string m_endpoint;

};


int main(int argc, char* argv[])
{
	std::cout << "start mdbroker" << std::endl;
	
	broker brk{};

	brk.bind("tcp://*:5555");
	
	brk.start_brocking();

	return 0;
}

