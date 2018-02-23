#include "master.hpp"
#include <sys/wait.h>

master::master()
{
	domain_endpoint = std::make_unique<DomainSocketClientEndpoint>("test.sock");
}

master::~master()
{
}


void master::process()
{
	printf("Master: %d workers are created\n", workers_pid.size());
	
	endpoint = std::make_unique<SctpServerEndpoint>("127.0.0.1", MY_PORT_NUM, io_multi);
	
	int i = 10;
	while(i--)
	{
		printf("[evan],start poll\n");
		sleep(1);
		io_multi.Poll();
	}
	
	wait_until_workers_closed();
	
	return;
}

void master::wait_until_workers_closed()
{
	while(workers_pid.size())
	{
		int status;
		int pid = wait(&status);
		
		std::vector<int>::iterator it;
		it = find(workers_pid.begin(), workers_pid.end(), pid);
		
		if(it != workers_pid.end())
		{
			workers_pid.erase(it);
			printf("[Master/PID: %d]: Worker stop with pid %d, Remain worker, %d!\n", getpid(), pid, workers_pid.size());
		}
	}
	printf("[Master/PID: %d]: Master Stop! \n", getpid());
	
	return;
}


void master::add_worker(int pid)
{
	workers_pid.push_back(pid);
}