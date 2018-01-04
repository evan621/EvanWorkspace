#include "zmq.hpp"

#include <string>
#include <iostream>


class zmsg {
public:
	zmsg(std::string str) {
		m_part_data.push_back(str);
	}
	
	zmsg(zmq::socket_t &socket)
	{
		recv(socket);
	}
	
	void send(zmq::socket_t *socket) 
	{
		std::cout << "send messge cnt = " << m_part_data.size() << std::endl;
		
		for (int part_nbr = 0; part_nbr < m_part_data.size(); part_nbr++)
		{
			std::cout << "send messge:" << m_part_data[part_nbr] << std::endl;

			s_send(socket, m_part_data[part_nbr], (part_nbr < m_part_data.size() - 1));
		}
		
	}
	
	bool recv(zmq::socket_t& socket) 
	{
		while(1)
		{
			zmq::message_t message(0);
			
			socket.recv(&message, 0);
			
			std::string msg_data;
						
			m_part_data.push_back(std::string(static_cast<char *>(message.data()), message.size()));
			
			if(!message.more()) {
				break;
			}
		}
		
		return true;
	}
	
	
	void dump(){
		std::cout << "-----------------------------------" << std::endl;
		
		for(unsigned int part_nbr = 0; part_nbr < m_part_data.size(); part_nbr++)
		{
			std::string data = m_part_data[part_nbr];
			
			std::cout << data << std::endl;
		}
	}
	
private:
	std::vector<std::string> m_part_data;
	
private:
	void s_send(zmq::socket_t *socket, std::string send_str, bool more)
	{
		zmq::message_t send_msg(send_str.size());
		memcpy(send_msg.data(), send_str.data(), send_str.size());
		
		socket->send(send_msg, more ? ZMQ_SNDMORE : 0);
	}
};


