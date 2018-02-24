#include "master.hpp"
#include <sys/wait.h>

master::master(std::vector<int> workers): 
        isMasterTerminated(false), workers_pid(workers)
{
    //Init logger
    logger = spdlog::basic_logger_mt("master", "./log/master.txt");
    logger->set_pattern("[%n][%P][%t][%l] %v");

    logger->info("master construct {}!", getpid());

    // init io multi
    io_multi        = std::make_shared<IoMultiplex>(logger);    
    test_endpoint   = std::make_unique<DomainSocketClientEndpoint>(TEST_MASTER_SOCKET_NAME, io_multi, logger);
    test_endpoint->register_handler([this](std::vector<char> msg){msg_handler(msg);});

    if(workers_pid.size() > 0)
    {
        //Create an ednpoint for communication with workers
        logger->info("{} Workers created, start worker endpoint!", workers_pid.size());
        worker_endpoint = std::make_unique<DomainSocketServerEndpoint>(MASTER_WORKER_SOCKET_NAME, io_multi, logger);
    }
    else
    {
        worker_endpoint = NULL;
        indicate_test_framework();
    }
}

master::~master()
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


void master::msg_handler(std::vector<char> msg)
{
    internal_msg msg_recv;
    std::memcpy(&msg_recv, msg.data(), msg.size());

    logger->info("Master msg_handler recv msg id/pid(%d, %x)\n", msg_recv.header.msg_id, msg_recv.master_ready.master_pid);

    switch(msg_recv.header.msg_id)
    {
        case WORKER_READY_MSG_ID:
            
            break;
        case MASTER_TERMINATE_MSG_ID:
            logger->info("Master is termincated!");
            isMasterTerminated = true;
            break;
        default:
            
            break;
    }

}

void master::indicate_test_framework()
{
    internal_msg msg;
    msg.header.msg_id = MASTER_READY_MSG_ID;
    msg.master_ready.master_pid = getpid();

    std::vector<char> serial_msg;

    serial_msg.resize(sizeof(msg));
    std::memcpy(serial_msg.data(), &msg, sizeof(msg));    
    test_endpoint->send_msg(serial_msg);

    logger->info("Master is ready, indicate test case!");
}

void master::send_terminate_to_client()
{
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

