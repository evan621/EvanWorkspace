#include "logger.hpp"


class MsgPollAndDispather{
public:
    void run();
    void registerMsgReceiver(int fd, IPort port);

private:
    std::vector<pollfd> poll_items;
    std::vector<IPort> port_lists;
    
    
private:

    void notifyMsgServicePort(int fd);

};


void MsgPollAndDispather::run()
{
    switch(poll(poll_items.data(), poll_items.size(), -1))
    {
        case -1:
            LOG_ERROR_MSG("Polling failed with error: %s", strerror(errno));
            break;
        case 0:
            LOG_ERROR_MSG("Polling Timeout !");
            break;
        default:
            break;
    }

    for (auto i = 0; i < poll_items.size(); i++)
    {
        if(poll_items.at(i).revents & POLLIN)
        {
            notifyMsgServicePort(poll_items.at(i).fd);
        }
        else
        {
            LOG_ERROR_MSG("Unexpected revents Received for fd, %d !", poll_items.at(i).fd);
        }
    }
}

void MsgPollAndDispather::notifyMsgServicePort(int fd)
{
    
}


void MsgPollAndDispather::registerMsgReceiver(int fd, IPort port)
{
    pollfd item;
    item.fd = fd;
    item.events = POLLIN;

    poll_items.push_back(item);
    port_lists.push_back(port);
}
