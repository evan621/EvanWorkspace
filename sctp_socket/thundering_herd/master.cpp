#include "master.hpp"
#include <sys/wait.h>

master::master(std::vector<int> workers): 
        isMasterTerminated(false), workers_pid(workers)
{
    logger = spdlog::basic_logger_mt("master", "./log/master.txt");
    logger->set_pattern("[%n][%P][%t][%l] %v");

    logger->info("master construct {}!", getpid());

    test_endpoint   = std::make_unique<DomainSocketClientEndpoint>(TEST_MASTER_SOCKET_NAME, logger);
    io_multi        = std::make_shared<IoMultiplex>(logger);

    if(workers_pid.size() > 0)
    {
        //Create an ednpoint for communication with workers
        worker_endpoint = std::make_unique<DomainSocketServerEndpoint>(MASTER_WORKER_SOCKET_NAME, io_multi, logger);
    }
    else
    {
        worker_endpoint = std:null_ptr;
        // No worker is created, master is ready. Indicate Test framwork
        indicate_test_framework();
    }
}

master::~master()
{
}

void master::indicate_test_framework()
{
    test_endpoint.send_msg();
}

void master::ready()
{
    while(worker_endpoint->get_client_num() < workers_pid.size())
    {
        io_multi->Poll();
    }
}


void master::prepare()
{
    //Wait until all the workers ready
    ready();
    logger->info("workers are all ready");

    //create sctp endpoint
    sctp_endpoint = std::make_unique<SctpServerEndpoint>("127.0.0.1", MY_PORT_NUM, io_multi, logger);
}

void master::send_terminate_to_client()
{
}


void master::run()
{
    while(!isMasterTerminated)
    {
        io_multi->Poll();
    }

    //Indicate workers to close
    send_terminate_to_client();

    //wait    
    wait_until_workers_closed();
}

void master::process()
{   
    logger->info("{} workers are created", workers_pid.size());

    prepare();

    //Wait for SCTP connection request
    int i = 2;
    while(i--)
    {
        printf("[evan],start poll, %x\n", io_multi);

        sleep(1);
        io_multi->Poll();
    }

    //Indicate workers to close
    send_terminate_to_client();

    //wait    
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
