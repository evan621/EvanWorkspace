//
//  Hello World server in C++
//  Binds REP socket to tcp://*:5555
//  Expects "Hello" from client, replies with "World"
//
#include <zmq.hpp>
#include <zhelpers.hpp>
#include <string>
#include <iostream>
#include <unistd.h>

#define SIZE_MESSAGE_MAX 400


int main (int argc,char *argv[])  
{
    if(argc <=1)
    {
      std::cout << "please input the server socket"<<std::endl;
      std::cout << "Example 1: ./hwserver ipc:///tmp/1 "<<std::endl;
      std::cout << "Example 2: ./hwserver tcp://192.168.122.65:5555 "<<std::endl;
      return 1;
    }
    s_version();
    //  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REP);
    
    socket.bind (argv[1]);
    std::cout<<"socket.bind: "<<argv[1]<<std::endl;

    while (true) {
        zmq::message_t request;
        int messageSize = SIZE_MESSAGE_MAX;

        //  Wait for next request from client
        socket.recv (&request);        
        //std::cout << "Received ..." <<((char *)request.data())[0]<<std::endl;
        
        //  Do some 'work'
    	  //sleep(1);

        //  Send reply back to client
        zmq::message_t reply (messageSize);
        memcpy (reply.data (), request.data(), messageSize);
        
        char value = ((char *)request.data())[0];
        value++; //change the value of the message
        memset(reply.data (), value, messageSize);
        socket.send (reply);
    }
    return 0;
}