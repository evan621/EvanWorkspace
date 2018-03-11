#include "worker.hpp"
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>


worker::worker():continue_poll(true), num_poll_item(0)
{
    std::string worker_name = "worker";
    std::string file_name = "./log/" + worker_name + std::to_string(getpid()) + ".txt";
    unlink(file_name.c_str());
    logger = spdlog::rotating_logger_mt(worker_name, file_name, 1024*1024, 1);
    logger->set_pattern("[%n][%P][%t][%l] %v");

    logger->info("Worker constructed!");

    io_multi = std::make_shared<IoMultiplex>(logger);
}

worker::~worker()
{   
    logger->info("worker destructed!");
}

void worker::msg_handler()
{
    int fd = -1; 
    auto serial_msg = master_socket_client->domain_recv_msg(fd);

    if(fd > 0)
    {
        printf("worker(%d) recv New sctp fd , %d\n", getpid(), fd);
        sctp_endpoint = std::make_unique<SctpClientEndpoint>(io_multi, logger, fd);
    }
    else
    {
        if(serial_msg == NULL)
        {
            printf("error, Worker recv null msg from master\n");

            // TODO: De register from poll
            exit(0);
            return;
        }
        
        internal_msg* msg = serial_msg->deserialized_data();
        logger->info("New message received from master, msg_id = {}!", msg->header.msg_id);

        int* data = (int  *)msg;
        printf("Worker msg_handler %d, %x, %d\n", msg->header.msg_id, msg, data[0]);

        switch(msg->header.msg_id)
        {
            case WORKER_TERMINATE_MSG_ID:
                printf("Worker termincate received\n");
                logger->info("worker is termincated by indication");
                continue_poll = false;
                break;
            default:
                logger->error("Unkown message id = {}", msg->header.msg_id);
                break;
        }
    }
}

void worker::send_worker_is_ready()
{
    internal_msg msg;
    msg.header.msg_id = WORKER_READY_MSG_ID;
    msg.worker_ready.worker_pid = getpid();

    auto serial_msg = std::make_unique<SerializedMsg<internal_msg>>(msg);
    master_socket_client->domain_write(std::move(serial_msg));
}

void worker::process()
{
    sleep(5);

    master_socket_client = std::make_unique<DomainSocket>(MASTER_WORKER_SOCKET_NAME, CLIENT_DOMAIN_SOCKET, logger);

    printf("[Info]: Worker(%d) connect to master with fd = %d\n", getpid(), master_socket_client->socket_fd());    

    io_multi->register_fd(master_socket_client->socket_fd(), [this](int sock_fd){
                    printf("worker read msg from socket, %d\n", sock_fd);
                    msg_handler();
                });
    send_worker_is_ready();

    logger->info("Worker start process!");

    while(continue_poll)
    {
        io_multi->Poll();
    }
}
