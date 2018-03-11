#include "IoMultiplex.hpp"
#include <errno.h>
#include <string.h>


void IoMultiplex::register_fd(int fd, CallBack cb)
{
    pollfd item;
    item.fd = fd;
    item.events = POLLIN;

    poll_items.push_back(item);
    
    cb_lists.push_back(cb);
}


void IoMultiplex::deregister_fd(int fd)
{
    for (auto i = 0; i < poll_items.size(); i++)
    {
        if(poll_items.at(i).fd == fd)
        {
            poll_items.erase(poll_items.begin()+i);
            cb_lists.erase(cb_lists.begin()+i);
            return;
        }
    }
}

void IoMultiplex::PollEventHandler()
{
    for(auto i = 0; i < poll_items.size(); i++)
    {
        // POLLIN, POLLRPI, POLLOUT, POLLERR, POLLHUP, POLLNVAL        
        if(poll_items.at(i).revents & POLLIN)
        {
            cb_lists.at(i)(poll_items.at(i).fd);
        } 
        else if(poll_items.at(i).revents & POLLERR )
        {
            printf("[Err PID = %d]: revents(POLLERR) during poll for fd, %d\n", getpid(), poll_items.at(i).fd);
        }
        else if(poll_items.at(i).revents & POLLHUP )
        {
            printf("[Err PID = %d]: revents(POLLHUP) during poll for %d\n", getpid(),  poll_items.at(i).fd);
        }
        else if(poll_items.at(i).revents & POLLNVAL )
        {
            printf("[Err PID = %d]: revents(POLLNVAL) during poll for %d! fd is not open.\n", getpid(),  poll_items.at(i).fd);
        }

    }
}

void IoMultiplex::Poll()
{
    switch(poll(poll_items.data(), poll_items.size(), -1))
    {
        case -1:
            printf("[Err PID = %d]: while polling: %s!\n", getpid(), strerror(errno));
            break;
        case 0:
            printf("[Err PID = %d]: Timeout While polling!\n",  getpid());
            break;
        default:
            PollEventHandler();
            break;
    }
}
