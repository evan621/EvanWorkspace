#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <memory>
#include "IoMultiplex.hpp"
#include "SctpClientEndpoint.hpp"
#include "DomainSocketServerEndpoint.hpp"

static 	IoMultiplex io_multi;

void start_sut()
{
	printf("[test]:start the sut!\n");
	
	execlp("./server", NULL);
}

void test_case()
{
	auto sctp_endpoint = std::make_unique<SctpClientEndpoint>(io_multi);

	sctp_endpoint->SendMsg();
	
	sleep(2);
	
	sctp_endpoint->SendMsg();
}

void indicate_master_to_quit()
{
}

void test_case_proc(int pid)
{
	printf("[test]: start the test case!\n");
	
	auto domain_endpoint = std::make_unique<DomainSocketServerEndpoint>("test.sock", io_multi);
	
	// wait for the connection request from SUT process.
	domain_endpoint->ready();
	
	// run test case
	test_case();
	
	// Indicate SUT to quit
	indicate_master_to_quit();
	
	// Wait sut to quit
	int status;
	if(wait(&status) == pid)
	{
		printf("[test]: the sut quit\n");
	}
}

int main()
{
	int pid = fork();
	
	switch(pid)
	{
		case 0:
			start_sut();
			break;
		case -1:
			break;
		default:
			test_case_proc(pid);
			break;
	}
}


