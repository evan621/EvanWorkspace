#include "worker.hpp"
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>


worker::worker()
{
    std::string worker_name = "worker";
    std::string file_name = "./log/" + worker_name + std::to_string(getpid()) + ".txt";
    logger = spdlog::basic_logger_mt(worker_name, file_name);
    logger->set_pattern("[%n][%P][%t][%l] %v");
    
    logger->info("worker constructed!");

    io_multi = std::make_shared<IoMultiplex>(logger);

    master_endpoint = std::make_unique<DomainSocketClientEndpoint>(MASTER_WORKER_SOCKET_NAME, logger);
}

worker::~worker()
{   
    logger->info("worker destructed!");
}

void worker::process()
{
    //io_multi.Poll();
    master_endpoint->send_msg();
        
    logger->info("hello!");
}