#include <iostream>
#include <vector>
#include <memory>
#include "worker.hpp"
#include "master.hpp"
#include <stdio.h>
#include <unistd.h>

#define PROCESS_NUM 3

int spawn_worker_process()
{
    int pid = fork();
    
    switch(pid)
    {
        case 0:
        {
            printf("[Worker/PID: %d]: New worker created by Parent (%d). \n", getpid(), getppid());
            auto newWorker = std::make_unique<worker>();
            newWorker->process();
            sleep(2);
            printf("[Worker/PID: %d]: Exit! \n", getpid());
            exit(0);
        }
        case -1:
        {
            printf("[Worker/PID: %d]: Fork failed!\n", getpid());
            exit(0);
        }
        default:
            return pid;
    }
}

int main()
{   
    auto master_proc = std::make_unique<master>();
    
    printf("[Master/PID: %d]: Master Start! \n", getpid());
    for (int i = 0; i < PROCESS_NUM; i++)
    {
        int worker_pid = spawn_worker_process();
        
        master_proc->add_worker(worker_pid);
    }
    
    master_proc->process();
    
    return 0;
}


