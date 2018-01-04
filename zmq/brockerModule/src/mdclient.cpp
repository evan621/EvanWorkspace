#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

#include <memory>

#include <time.h>
#include <assert.h>
#include <stdlib.h>        // random()  RAND_MAX
#include <stdio.h>
#include <stdarg.h>
#include <signal.h>
#include <unistd.h>


#include "zmsg.hpp"

#define within(num) (int) ((float) (num) * random () / (RAND_MAX + 1.0))

inline std::string
s_set_id (zmq::socket_t & socket)
{
    std::stringstream ss;
    ss << std::hex << std::uppercase
       << std::setw(4) << std::setfill('0') << within (0x10000) << "-"
       << std::setw(4) << std::setfill('0') << within (0x10000);
    socket.setsockopt(ZMQ_IDENTITY, ss.str().c_str(), ss.str().length());
    return ss.str();
}

class mdcli
{
public:
	mdcli(std::string broker):m_socket(nullptr)
	{
		m_context = std::make_shared<zmq::context_t>(1);
		m_socket = new zmq::socket_t(*m_context, ZMQ_REQ);
		
		s_set_id(*m_socket);
		int linger = 0;
        m_socket->setsockopt(ZMQ_LINGER, &linger, sizeof (linger));

		m_socket->connect(broker.c_str());
		
		std::cout << "[mdcli]: connect to client, socket = " << *m_socket << std::endl;
	}
	
	
	void send(zmsg* request)
	{
		request->send(m_socket);
	}

private:
	std::shared_ptr<zmq::context_t> m_context;
	zmq::socket_t* m_socket;
};



int main(int argc, char* argv[])
{
	mdcli session{"tcp://localhost:5555"};

	zmsg* request = new zmsg("Hello world");

	// If the app quite immediatly afte sending the request, the messages could be lost. 
	session.send(request);
	
	int cnt = 10;
	while(cnt)
	{
		cnt--;
		usleep(1000);
	}
	

	return 0;
}