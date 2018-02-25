#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <memory>
#include "IoMultiplex.hpp"
#include "SctpClientEndpoint.hpp"
#include "DomainSocketServerEndpoint.hpp"
#include "spdlog/spdlog.h"
#include "common.h"

std::shared_ptr<IoMultiplex> io_multi;
std::shared_ptr<spdlog::logger> logger;
std::unique_ptr<DomainSocketServerEndpoint> test_endpoint;


void start_sut()
{
    printf("[test]:start the sut!\n");
    
    execlp("./server", NULL);
}

void test_case()
{
    auto sctp_endpoint = std::make_unique<SctpClientEndpoint>(io_multi, logger);

    sctp_endpoint->SendMsg();
    
    sleep(2);
    
    sctp_endpoint->SendMsg();
}

void indicate_master_to_quit()
{
}

void ready()
{
    while(test_endpoint->get_client_num() != 1)
    {
        io_multi->Poll();
    }
}

void test_case_proc(int pid)
{
    printf("[test]: start the test case!\n");
    
    logger = spdlog::basic_logger_mt("test", "./log/test.txt");
    logger->set_pattern("[%n][%P][%t][%l] %v");
    
    io_multi = std::make_shared<IoMultiplex>(logger);
    test_endpoint = std::make_unique<DomainSocketServerEndpoint>(TEST_MASTER_SOCKET_NAME, io_multi, logger);

    printf("[test]: wait for sut ready...!\n");
    // wait for the connection request from SUT process.
    ready();

    
    printf("[test]: Start test case...!\n");
    // run test case
    test_case();

    
    printf("[test]: Indicate sut to quit...!\n");
    // Indicate SUT to quit
    indicate_master_to_quit();

    
    printf("[test]: Wait sut to quit...!\n");
    // Wait sut to quit
    int status;
    if(wait(&status) == pid)
    {
        printf("[test]: the sut quit\n");
    }
}

void prepare_test_env()
{
    std::cout << "Prepare the Test env!" << std::endl;

    if(access(TMP_DIR, 0) != 0) 
    {
        if(0 == mkdir(TMP_DIR, 0777))
        {
            printf("[Test]: Dir(%s) created!\n", TMP_DIR);
        }
        else
        {
            exit(-1);
        }
    }
    
    if(access(LOG_DIR, 0) != 0) 
    {
        if(0 == mkdir(LOG_DIR, 0777))
        {
            printf("[Test]: Dir(%s) created!\n", LOG_DIR);
        }
        else
        {
            exit(-1);
        }
    }

}

int main()
{
    prepare_test_env();


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


