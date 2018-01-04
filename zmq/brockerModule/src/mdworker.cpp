#include <stdlib.h>
#include <iostream>


class mdwrk
{
public:
	mdwrk(std::string broker)
	{
		m_context = new zmq::context_t(1)
		m_worker = new zmq::socket_t(*m_context, ZMQ_DEALER);

		m_worker-> connect (broker.c_str());
	}

	void recv(zmsg &reply)
	{
		zmq::poll();
	}
private:

	zmq::context_t *m_context;
	zmq::socket_t *m_worker;

};




int main(int argc, char* argv[])
{
	mdwrk session ("tcp://localhost::5555");

	zmsg reply;

	while(1) {
		session.recv(reply);
	}
	return 0;
}