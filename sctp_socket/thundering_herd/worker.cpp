#include "worker.hpp"
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>


worker::worker()
{
	printf("[Worker/PID: %d]: worker constructed\n", getpid());
}

worker::~worker()
{
	printf("[Worker/PID: %d]: worker destructed\n", getpid());
}

void worker::process()
{
	//io_multi.Poll();
	
	printf("[Worker/PID: %d]: hello\n", getpid());
}