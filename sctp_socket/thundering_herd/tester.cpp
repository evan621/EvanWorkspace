#include "tester.hpp"

#define MAX_NUM_CONNECTION 1


tester::tester(): continue_poll(true), is_sut_ready(false)
{
    unlink("./log/test.txt");

    logger = spdlog::rotating_logger_mt("test", "./log/test.txt", 1024*1024, 1);
    logger->set_pattern("[%n][%P][%t][%l] %v");

    io_multi = std::make_shared<IoMultiplex>(logger);
    test_socket_server_listen = std::make_unique<DomainSocket>(TEST_MASTER_SOCKET_NAME, SERVER_DOMAIN_SOCKET, logger);

    io_multi->register_fd(test_socket_server_listen->socket_fd(), [this](int sock_fd){
                            sut_connect_handler();
                        });
 
    io_multi->register_fd(STDIN, [this](int fd) 
                        {  ReadUserCmd(fd); } );
}

tester::~tester()
{}

void tester::sut_connect_handler()
{
    int conn_socket_fd = test_socket_server_listen->domain_accept();
    test_socket_server_connect = std::make_unique<DomainSocket>(conn_socket_fd, logger);

    io_multi->register_fd(test_socket_server_connect->socket_fd(), [this](int sock_fd){
        
                printf("new sut message received request %d\n", sock_fd);
                int fd;
                auto msg = test_socket_server_connect->domain_recv_msg(fd);

                if(msg == NULL)
                {
                    printf("[Info PID = %d]: Connection to SUT lost, deregister fd(%d)\n", getpid(), sock_fd);

                    io_multi->deregister_fd(sock_fd);
                    return;
                }
                test_msg_handler(msg->deserialized_data());
            });
}

void tester::ReadUserCmd(int fd)
{
    char buf;  
    read(fd, &buf, sizeof(buf));
    switch(buf)
    {
        case '0':
            continue_poll = false;
            break;
        case '1':
            if(!is_sut_ready)
            {
                printf("SUT is not ready, wait...\n");
                sleep(2);
            }
            else
            {
                printf("Start test...\n");
                test_case();
                printf("Test end!\n");
            }
            
            print_instruct();
        default:
            break;
    }
}

void tester::print_instruct()
{
    printf("[0]: Enter 0 to to exit!\n");
    printf("[1]: Enter 1 to to test!\n");
}
void tester::run()
{    
    printf("Start! Wait SUT ready...\n");

    while(continue_poll)
    {
        io_multi->Poll();
    }

    terminate();
}

void tester::terminate()
{
    printf("[TEST]: Indicate sut to quit...!\n");
    // Indicate SUT to quit
    indicate_sut_to_quit();

    // Wait sut to quit
    int status;
    if(wait(&status))
    {
        printf("[TEST]: the sut quit\n");
    }
}

void tester::test_case()
{
    for (int i = 0; i < MAX_NUM_CONNECTION; i++)
    {
        printf("start endpoint: %x\n", i);
        auto endpoint = std::make_shared<SctpClientEndpoint>("127.0.0.1", MY_PORT_NUM, io_multi, logger);

        sctp_endpoints.push_back(endpoint);
        
    }

    for (int msg_cnt = 0; msg_cnt < 1; msg_cnt++)
    {
        std::string hello = "helloworld: !" + std::to_string(msg_cnt);

        std::vector<char> msg(hello.begin(), hello.end());
        for (int i = 0; i < MAX_NUM_CONNECTION; i++)
        {
            printf("send message to: %x\n",  i);
            sctp_endpoints.at(i)->SendMsg(msg);
        }
    }
}

void tester::indicate_sut_to_quit()
{
    internal_msg msg;
    msg.header.msg_id = MASTER_TERMINATE_MSG_ID;

    printf("[TEST] Indicate sut to quit\n");
    auto serial_msg = std::make_unique<SerializedMsg<internal_msg>>(msg);
    test_socket_server_connect->domain_write(std::move(serial_msg));
}

void tester::test_msg_handler(internal_msg* msg_recv)
{
    switch(msg_recv->header.msg_id)
    {
        case(MASTER_READY_MSG_ID):
            printf("SUT is ready!\n");
            is_sut_ready = true;
            printf("Start test...\n");
            test_case();      
            print_instruct();
            break;
        default:
            logger->error("Received unknown message from domain socket!");
            break;
    }

}
