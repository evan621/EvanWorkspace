#ifndef _DOMAIN_SOCKET_HPP
#define _DOMAIN_SOCKET_HPP

#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <string.h>
#include <memory>
#include <vector>
#include "IoMultiplex.hpp"
#include "spdlog/spdlog.h"
#include "common.hpp"


#define SERVER_DOMAIN_SOCKET 90
#define CLIENT_DOMAIN_SOCKET 91

template<typename msg_type>
class SerializedMsg
{
public:
    SerializedMsg(msg_type msg)
    {
        serial_msg = std::make_unique<std::vector<char>>(sizeof(msg), 0);
        std::memcpy(serial_msg->data(), &msg, sizeof(msg));    
    }
    SerializedMsg(char* msg_ptr, int size)
    {
        serial_msg = std::make_unique<std::vector<char>>(msg_ptr, msg_ptr + size);
    }
    ~SerializedMsg(){};

    char* data(){return serial_msg->data();}
    int size(){return serial_msg->size();}
    msg_type* deserialized_data(){return (msg_type*)(serial_msg->data());}
    
private:
    std::unique_ptr<std::vector<char>> serial_msg;
};


class DomainSocket
{
public:
    DomainSocket(std::string servername, int socketType, std::shared_ptr<spdlog::logger> logger);
    DomainSocket(int fd, std::shared_ptr<spdlog::logger> logger);
    ~DomainSocket();

    int domain_accept();
    
    std::unique_ptr<SerializedMsg<internal_msg>> domain_read();
    void domain_write(std::unique_ptr<SerializedMsg<internal_msg>> msg);
    int socket_fd(){return sock_fd;};

    void domain_send_msg(std::unique_ptr<SerializedMsg<internal_msg>> serial_msg);
    int domain_send_fd(int fd);
    
    std::shared_ptr<SerializedMsg<internal_msg>> domain_recv_msg(int& recvfd);
private:
    int sock_fd;
    std::shared_ptr<spdlog::logger> logger;
        
    void domain_create();
    void domain_bind(std::string servername);
    int domain_listen();
    int domain_connect(std::string servername);
    void domain_close();

};


#endif
