#include "worker.hpp"
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>


worker::worker()
{
    std::string worker_name = "worker";
    std::string file_name = LOG_DIR + worker_name + std::to_string(getpid()) + ".txt";
    logger = spdlog::basic_logger_mt(worker_name, file_name);
    logger->set_pattern("[%n][%P][%t][%l] %v");
    
    logger->info("worker constructed!");

    io_multi = std::make_shared<IoMultiplex>(logger);
}

worker::~worker()
{   
    logger->info("worker destructed!");
}

void worker::process()
{
    //io_multi.Poll();
        
    logger->info("hello!");
}