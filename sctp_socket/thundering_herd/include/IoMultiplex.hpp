#ifndef _IO_MULTIPLEX
#define _IO_MULTIPLEX

#include <iostream>
#include <functional>
#include <vector>
#include <poll.h>
#include "spdlog/spdlog.h"

typedef std::function<void(int fd)> CallBack;

class IoMultiplex
{
public:
    IoMultiplex(std::shared_ptr<spdlog::logger> logger):logger(logger){}
    ~IoMultiplex(){}
    
    void RegisterFd(int fd, CallBack cb);
    void Poll();
    
private:
    void PollEventHandler();
    
    std::vector<pollfd> poll_items;
    std::vector<CallBack> cb_lists;

    std::shared_ptr<spdlog::logger> logger;
};

#endif