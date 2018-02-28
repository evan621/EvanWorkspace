#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <memory>
#include "IoMultiplex.hpp"
#include "SctpClientEndpoint.hpp"
#include "DomainSocketServerEndpoint.hpp"
#include "spdlog/spdlog.h"
#include "common.hpp"
#include "tester.hpp"

void start_sut()
{    
    execlp("./server", NULL);
}

void prepare_test_env()
{
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

void start_test()
{
    tester test;
    test.run();
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
            start_test();
            break;
    }
}


