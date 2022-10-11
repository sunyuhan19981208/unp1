#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "sum.h"

void str_echo(int fd) {
    ssize_t n;
    char buf[32];
    args temp;
    sum res;
    while((n = read(fd, &temp, sizeof(args)))>0) {
        res.res = temp.arg1 + temp.arg2;
        int len = write(fd, &res, sizeof(res));
    }
    if(n < 0) {
        std::cerr << "read error" << std::endl;
        exit(1);
    }
    std::cout <<"end lo" << std::endl;
}

void solve_sigpipe(int sig){
    std::cout << "SIGPIPE" << std::endl;
}
int main() {
    signal(SIGPIPE, solve_sigpipe);
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    sockaddr_in cliaddr, servaddr;
    
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(8888);

    bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr));

    listen(listenfd, 10);

    while(true) {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (sockaddr*)&cliaddr, &clilen);
        pid_t pid;
        if(0 == (pid=fork())) {
            close(listenfd);
            str_echo(connfd);
            exit(0);
        }
        close(connfd);
    }
}