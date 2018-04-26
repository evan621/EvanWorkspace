#include <iostream>
#include <functional>
#include <vector>
#include <poll.h>

typedef std::function<void(int fd)> CallBack;

class IoMultiplex
{
public:
	IoMultiplex(){}
	~IoMultiplex(){}
	
	void RegisterFd(int fd, CallBack cb);
	void Poll();
	
private:
	void PollEventHandler();
	
	std::vector<pollfd> poll_items;
	std::vector<CallBack> cb_lists;
};