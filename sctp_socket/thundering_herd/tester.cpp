#include "tester.hpp"

tester::tester(): continue_poll(true)
{
    logger = spdlog::basic_logger_mt("test", "./log/test.txt");
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
            printf("[TEST]: Exit test! \n"); 
            continue_poll = false;
            break;
        default:
            printf("[TEST]: Unrecognized command!\n");
            break;
    }
}


void tester::run()
{
    printf("[TEST]: start the test case!\n");
    
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
    auto sctp_endpoint = std::make_unique<SctpClientEndpoint>(io_multi, logger);

    sctp_endpoint->SendMsg();
    
    sleep(2);
    
    sctp_endpoint->SendMsg();
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

    printf("[TEST] recv_msg size(%d) id/pid(%d, %x)\n", 
                msg.size(), msg_recv.header.msg_id, msg_recv.master_ready.master_pid);

    //continue_poll = false;
}
