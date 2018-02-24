#include "master.hpp"
#include <sys/wait.h>

master::master()
{
    logger = spdlog::basic_logger_mt("master", "./log/master.txt");
    logger->set_pattern("[%n][%P][%t][%l] %v");

    domain_endpoint = std::make_unique<DomainSocketClientEndpoint>(TEST_MASTER_SOCKET_NAME, logger);
    io_multi = std::make_shared<IoMultiplex>(logger);
}

master::~master()
{
}


void master::process()
{   
    logger->info("{} workers are created", workers_pid.size());
    
    endpoint = std::make_unique<SctpServerEndpoint>("127.0.0.1", MY_PORT_NUM, io_multi, logger);

    //Wait until all the workers ready
    
    
    int i = 2;
    while(i--)
    {
        printf("[evan],start poll, %x\n", io_multi);

        sleep(1);
        io_multi->Poll();
    }
    
    printf("[evan],end poll\n");
    
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
            logger->info("Worker stop with pid {}, Remain worker, {}!", pid, workers_pid.size());

        }
    }   
    logger->info("Master Stop! \n");
    
    return;
}


void master::add_worker(int pid)
{
    workers_pid.push_back(pid);
}
