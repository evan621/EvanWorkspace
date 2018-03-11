#ifndef _COMMON
#define _COMMON

#include <functional>
#include <vector>

#define MAX_BUFFER	256

#define MY_PORT_NUM	18002

#define LOCALTIME_STREAM	0
#define GMT_STREAM		1

#define LISTEN_QUEUE 5

#define TIME_OUT 10000

#define MAX_CONNECTION_NUMBER 10


#define STDIN 0

//#define ENABLE_ALL_NOTIFICATION
#define TMP_DIR "./tmp"
#define LOG_DIR "./log"
#define TEST_MASTER_SOCKET_NAME "./tmp/test_master.sock"
#define MASTER_WORKER_SOCKET_NAME "./tmp/master_worker.sock"

//Define the msg IDs
#define MASTER_READY_MSG_ID 0
#define WORKER_READY_MSG_ID 1
#define MASTER_TERMINATE_MSG_ID 2
#define WORKER_TERMINATE_MSG_ID 3


typedef struct
{
    int msg_id;
    int master_pid;
}master_ready_msg;

typedef struct
{
    int msg_id;
    int placeholer;
}master_terminate_msg;

typedef struct
{   
    int msg_id;
    int worker_pid;
}worker_ready_msg;

typedef struct
{   
    int msg_id;
    int placeholder;
}worker_terminate_msg;


typedef union
{
    struct {
        int msg_id;
    } header;
    master_ready_msg        master_ready;
    master_terminate_msg    master_termincate;
    worker_ready_msg        worker_ready;
    worker_terminate_msg    worker_terminate;
    
}internal_msg;

typedef std::function<void(std::vector<char>)> DomainSocketCallBack;
typedef std::function<void(int newfd)> newconn_handler_callbk;


#endif

