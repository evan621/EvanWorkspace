#ifndef _MASTER
#define _MASTER

#include <stdio.h>
#include <vector>
#include <algorithm>
#include <sys/wait.h>
#include <unistd.h>
#include <memory>
#include "SctpServerEndpoint.hpp"
#include "IoMultiplex.hpp"
#include "DomainSocketClientEndpoint.hpp"

class master
{
public:
	master();
	~master();
	
	void process();
	
	void add_worker(int pid);
	
private:
	void wait_until_workers_closed();

	std::unique_ptr<SctpEndpoint> endpoint;
	std::unique_ptr<DomainSocketClientEndpoint> domain_endpoint;

	IoMultiplex io_multi;
	std::vector<int> workers_pid;

};



#endif