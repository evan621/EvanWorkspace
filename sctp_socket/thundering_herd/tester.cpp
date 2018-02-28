#include "tester.hpp"

tester::tester(): continue_poll(true), is_sut_ready(false)
{
    unlink("./log/test.txt");

    logger = spdlog::rotating_logger_mt("test", "./log/test.txt", 1024*1024, 1);
    logger->set_pattern("[%n][%P][%t][%l] %v");

    io_multi = std::make_shared<IoMultiplex>(logger);
    test_endpoint = std::make_unique<DomainSocketServerEndpoint>(TEST_MASTER_SOCKET_NAME, io_multi, logger);
    test_endpoint->register_handler([this](std::vector<char> msg) 
                        { test_msg_handler(msg); });

    io_multi->RegisterFd(STDIN, [this](int fd) 
                        {  ReadUserCmd(fd); } );
}

tester::~tester()
{}

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
    auto sctp_endpoint = std::make_unique<SctpClientEndpoint>("127.0.0.1", MY_PORT_NUM, io_multi, logger);

    std::string hello = "helloworld!";
    std::vector<char> msg(hello.begin(), hello.end());
    
    sctp_endpoint->SendMsg(msg);

    /*
    sleep(2);
    
    sctp_endpoint->SendMsg(msg);*/
}

void tester::indicate_sut_to_quit()
{
    internal_msg msg;
    msg.header.msg_id = MASTER_TERMINATE_MSG_ID;

    std::vector<char> serial_msg; 

    serial_msg.resize(sizeof(msg));
    std::memcpy(serial_msg.data(), &msg, sizeof(msg));

    printf("[TEST] Indicate sut to quit\n");
    test_endpoint->publish_msg(serial_msg);
}

void tester::test_msg_handler(std::vector<char> msg)
{
    internal_msg msg_recv;

    std::memcpy(&msg_recv, msg.data(), msg.size());

    switch(msg_recv.header.msg_id)
    {
        case(MASTER_READY_MSG_ID):
            printf("SUT is ready!\n");
            is_sut_ready = true;
            print_instruct();
            break;
        default:
            logger->error("Received unknown message from domain socket!");
            break;
    }

}
