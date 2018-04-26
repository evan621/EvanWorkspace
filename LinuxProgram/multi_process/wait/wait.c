#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#ifndef CHILD_COUNT
    #define CHILD_COUNT 10000
#endif
static void printTime() {
    time_t calendar_time = time(NULL);
    struct tm * tm_local = localtime(&calendar_time);
    char str_f_t [50];
    strftime(str_f_t, sizeof(str_f_t), "%G-%m-%d %H:%M:%S", tm_local);
    printf("%s ", str_f_t);
}
int main/*09*/(int argc, char *argv[]) {
    pid_t cpid, w;
    int status;
    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (cpid == 0) {
        /* Code executed by child */
        FILE* fChild = freopen("/Users/sodino/workspace/xcode/Define/Define/child.txt", "w", stdout);
        printTime();
        printf("Child PID is %ld  argc=%d\n", (long) getpid(), argc);
        int count = 0;
        do{
            sleep(1);
            printTime();
            printf("sleep count=%d\n", count);
            fflush(fChild);
            
            count ++;
            if (count > CHILD_COUNT) {
                break;
            }
        }while (1);
        fflush(fChild);
        fclose(fChild);
        
        // code值将从父进程的WEXITSTATUS()宏定义中获知
//      _exit(123);
        exit(123);
    } else {
        FILE * fMain = freopen("/Users/sodino/workspace/xcode/Define/Define/main.txt", "w", stdout);
        
        // 如果子进程还在，则返回0；如果子进程已经运行结束，则返回子进程pid
        w = waitpid(cpid, &status, WNOHANG);
        if (w == 0) {
            printTime();
            printf("Child PID=%d has not yet changed state\n", cpid);
        } else {
            printTime();
            printf("Child PID=%d has been terminated.\n", cpid);
        }
        
        int ifExited, ifSignaled, ifStopped, ifContinued;
        /* Code executed by parent */
        do {
            // 在mac上·WCONTINUED·无效，从头文件来看，只适用于thread
            w = waitpid(cpid, &status, WUNTRACED | WCONTINUED);
//            w = waitpid(cpid, &status, WUNTRACED);
            if (w == -1) {
                printTime();
                printf("Parent w=-1, error=%s \n", strerror(errno));
                exit(EXIT_FAILURE);
            } if (w == 0) {
                printTime();
                printf("w == 0 ignore. continue.\n");
                continue;
            }
            
            ifExited = WIFEXITED(status);
            ifSignaled = WIFSIGNALED(status);
            ifStopped = WIFSTOPPED(status);
            ifContinued = WIFCONTINUED(status);
            
            printTime();
            printf("pid=%ld w=%d exitCode=%d status=%d ifExited=%d ifSignaled=%d ifStopped=%d ifContinued=%d \n", (long)getpid(),
//                   w, status, WEXITSTATUS(w), ifExited, ifSignaled, ifStopped, ifContinued);
                   w, status, _WSTATUS(w), ifExited, ifSignaled, ifStopped, ifContinued);
            
            printTime();
            if (ifExited) {
                printf("PID=%ld exited, status=%d\n", (long)w, WEXITSTATUS(status));
            } else if (ifSignaled) {
                printf("PID=%ld killed by signal %d\n", (long)w, WTERMSIG(status));
            } else if (ifStopped) {
                printf("PID=%ld stopped by signal %d\n", (long)w, WSTOPSIG(status));
            } else if (ifContinued) {
                printf("PID=%ld continued\n", (long)w);
            }
            fflush(fMain);
            if (ifExited || ifSignaled) {
                printTime();
                printf("isExited=%d   isSingaled=%d\n", ifExited, ifSignaled);
                fflush(fMain);
                break;
            }
        } while (1);
        
        printTime();
        printf("Main PID %ld  exit.\n", (long)getpid());
        
        fclose(fMain);
        exit(EXIT_SUCCESS);
    }
}