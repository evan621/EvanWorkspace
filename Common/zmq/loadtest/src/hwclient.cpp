//
//  Hello World client in C++
//  Connects REQ socket to tcp://localhost:5555
//  Sends "Hello" to server, expects "World" back
//
#include <zmq.hpp>
#include <string>
#include <iostream>
#include <sys/time.h>

#define SIZE_MESSAGE_MAX 400
char chargingContext[SIZE_MESSAGE_MAX];

int main (int argc,char *argv[])
{
    
    if(argc <=1)
    {
      std::cout << "please input the server socket"<<std::endl;
      std::cout << "Example 1: ./hwclient ipc:///tmp/1 "<<std::endl;
      std::cout << "Example 2: ./hwclient tcp://192.168.122.65:5555 "<<std::endl;
      return 1;
    }
    int request_count = 5;
    if(argc==3)
      request_count = atoi(argv[2]);
      
    //  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REQ);
    
    socket.connect (argv[1]);     
    std::cout << "Connecting to server..." <<argv[1]<<std::endl;    
    
    struct timeval tv_begin, tv_end;
    double sumTime = 0;
    int timeCount = 0;
    gettimeofday(&tv_begin, NULL);
    //std::cout<<"beginTime:"<<tv_begin.tv_sec<<","<<tv_begin.tv_usec<<std::endl;
    std::cout<<"start sending and receiving messages...repeat time:"<<request_count<<std::endl;
    //  Do 10 requests, waiting each time for a response
    for (int request_nbr = 0; request_nbr <100000*request_count; request_nbr++) {
        int messageSize = SIZE_MESSAGE_MAX;
        zmq::message_t request (messageSize);
        
        char temp = 'a'+(request_nbr%26);
        memset(chargingContext, temp, messageSize);        
        memcpy (request.data (), chargingContext, messageSize);
        
        socket.send (request);

        //  Get the reply.
        zmq::message_t reply;
        socket.recv (&reply);
        if(request_nbr%100000 == (100000-1))
        {
          gettimeofday(&tv_end, NULL);
          std::cout<<"Send&Receive 100000 messages, the time in microSecond(0.000001): "<<(tv_end.tv_sec*1000000+tv_end.tv_usec)-(tv_begin.tv_sec*1000000+tv_begin.tv_usec)<<std::endl;
          sumTime += (tv_end.tv_sec*1000000+tv_end.tv_usec)-(tv_begin.tv_sec*1000000+tv_begin.tv_usec);
          timeCount ++;
          gettimeofday(&tv_begin, NULL);//udpate the begin time with latest time
        }
    }
    std::cout<<"Send&Receive 100000 messages, the average time in Second: "<<sumTime/timeCount/1000000<<std::endl;
    return 0;
}
