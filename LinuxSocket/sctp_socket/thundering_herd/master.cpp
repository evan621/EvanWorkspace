#include "master.hpp"
#include <sys/wait.h>

master::master(std::vector<int> workers): 
        isMasterTerminated(false), workers_pid(workers), worker_count(0)
{
    //Init logger(rotating log, max size is 1MB, 1 file)
    unlink("./log/master.txt");
    logger = spdlog::rotating_logger_mt("master", "./log/master.txt", 1024*1024, 1);
    logger->set_pattern("[%n][%P][%t][%l] %v");

    logger->info("master constructed {}!", getpid());

    // init io multi
    io_multi            = std::make_shared<IoMultiplex>(logger);    
    test_socket_client  = std::make_unique<DomainSocket>(TEST_MASTER_SOCKET_NAME, CLIENT_DOMAIN_SOCKET, logger);

    printf("Master(%d) create new client fd(%d) connect to tester\n", getpid(), test_socket_client->socket_fd());


    io_multi->register_fd(test_socket_client->socket_fd(), [this](int sock_fd){
                                    printf("Master(%d) recv msg from test with fd: %d\n", getpid(), sock_fd);
                                    if(sock_fd != test_socket_client->socket_fd())
                                    {
                                        logger->error("Incorrect socket fd: {} for test_socket_client", sock_fd);
                                        return;
                                    }
                                    auto msg = test_socket_client->domain_read();

                                    if(msg == NULL)
                                    {
                                        // TODO: recv return 0, peer disconnected, de registered from poll
                                        logger->error("Message received on fd {} is null", sock_fd);
                                        exit(0);
                                    }
                                    msg_handler(msg->deserialized_data());
                                });

    if(workers_pid.size() > 0)
    {
        printf("workers pid: %d, num: %d\n", workers_pid[0], workers_pid.size());
    
        //Create an ednpoint for communication with workers
        logger->info("{} Workers created, start worker endpoint!", workers_pid.size());
        worker_socket_listen = std::make_unique<DomainSocket>(MASTER_WORKER_SOCKET_NAME, SERVER_DOMAIN_SOCKET, logger);

        printf("Master(%d) create listener fd (%d) wait connection from worker", getpid(), worker_socket_listen->socket_fd());
        io_multi->register_fd(worker_socket_listen->socket_fd(), [this](int sock_fd){
                    
                    int conn_fd = worker_socket_listen->domain_accept();
                    woker_connect_handler(conn_fd);
                    printf("Master(%d) detect new conn from worker, create fd(%d)\n", getpid(), conn_fd);
                });
    }
    else
    {
        worker_socket_listen = NULL;
        master_ready_forwork();
    }
}

master::~master()
{
}

void master::woker_connect_handler(int conn_fd)
{
    auto worker_socket = std::make_unique<DomainSocket>(conn_fd, logger);

    logger->info("Master detect new connection from worker!");
    
    io_multi->register_fd(conn_fd, [this](int sock_fd){
                worker_msg_handler(sock_fd);
            });

    workers_socket_connect.insert(std::pair<int, std::unique_ptr<DomainSocket>>(conn_fd, std::move(worker_socket)));
}

void master::worker_msg_handler(int fd)
{
    auto serial_msg = workers_socket_connect.at(fd)->domain_read();

    if(serial_msg == NULL)
    {
        printf("[Err PID = %d] Worker deregister fd = %d from poll\n", getpid(), fd);
        io_multi->deregister_fd(fd);

        // TODO: Remove the worker from workers_socket_connect. 
        return;
    }

    internal_msg* msg = serial_msg->deserialized_data();

    switch(msg->header.msg_id)
    {
        case WORKER_READY_MSG_ID:
            printf("Worker is ready received, pid %d\n", msg->worker_ready.worker_pid);
        
            worker_count++;
            logger->info("Worker(PID: {}) is ready", msg->worker_ready.worker_pid);
            break;
        default:
            logger->error("Unkown message: {} received from worker {}", msg->header.msg_id, fd);
            break;
    }

    if (worker_count == workers_pid.size())
    {
        master_ready_forwork();
    }
}

void master::run()
{
    while(!isMasterTerminated)
    {
        io_multi->Poll();
    }

    //Indicate workers to close
    send_terminate_to_worker();

    //wait    
    wait_until_workers_closed();
}


void master::msg_handler(internal_msg *msg_recv)
{

    logger->info("Master msg_handler recv msg id/pid({}, {})\n", msg_recv->header.msg_id, msg_recv->master_ready.master_pid);

    switch(msg_recv->header.msg_id)
    {
        case MASTER_TERMINATE_MSG_ID:
            logger->info("Master is termincated!");
            isMasterTerminated = true;
            break;
        default:
            
            break;
    }

}

void master::master_ready_forwork()
{
    printf("Master is ready for work\n");

    //start SCTP server endpoint, waiting for request
    std::string serverIP("127.0.0.1");
    sctp_endpoint = std::make_unique<SctpServerEndpoint>(serverIP, MY_PORT_NUM, io_multi, logger);

    sctp_endpoint->register_newconn_handler([this](int fd) {
                    printf("new connected fd created on master, %d \n", fd);
                    std::map<int, std::unique_ptr<DomainSocket>>::iterator it; 
                    for(it = workers_socket_connect.begin(); it != workers_socket_connect.end(); ++it)
                    {
                        printf("Send fd to worker, %d\n", it->first);
                        it->second->domain_send_fd(fd);
                    }
                    
                    close(fd);
                });
    
    logger->info("master create SCTP server endpoint!");

    // Tell test framework sut is ready
    indicate_test_framework();
}

void master::indicate_test_framework()
{
    internal_msg msg;
    msg.header.msg_id = MASTER_READY_MSG_ID;
    msg.master_ready.master_pid = getpid();

    logger->info("Master is ready, indicate test framework!");

    auto serial_msg = std::make_unique<SerializedMsg<internal_msg>>(msg);
    //test_socket_client->domain_write(std::move(serial_msg));
    test_socket_client->domain_send_msg(std::move(serial_msg));
}

void master::send_terminate_to_worker()
{
    internal_msg msg;
    msg.header.msg_id = WORKER_TERMINATE_MSG_ID;

    printf("Send Termincate to worker\n");

    auto serial_msg = std::make_unique<SerializedMsg<internal_msg>>(msg);

    std::map<int, std::unique_ptr<DomainSocket>>::iterator it; 
    for(it = workers_socket_connect.begin(); it != workers_socket_connect.end(); ++it)
    {
        printf("Send termincate to worker, %d\n", it->first);
        it->second->domain_write(std::move(serial_msg));
    }
}

void master::wait_until_workers_closed()
{
    while(workers_pid.size())
    {
        int status;
        int pid = wait(&status);

        printf("process quit with pid, %d\n", pid);
        
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

