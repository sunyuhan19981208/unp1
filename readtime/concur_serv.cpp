#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <chrono>
#define MAX 1024

int main(int argc, char **argv)
{
    int listenfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    char buff[MAX];
    time_t ticks;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket fail");
        return 1;
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(8888); /* daytime server */

    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    listen(listenfd, 10);

    while (1)
    {
        socklen_t len = sizeof(cliaddr);
        // sockaddr common
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len);
        pid_t pid;
        if((pid = fork()) == 0){
            close(listenfd);
            printf("connection from %s, port %d\n", inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)), ntohs(cliaddr.sin_port));
            std::cout << connfd << " " << listenfd << std::endl;
            ticks = time(NULL);
            snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
            write(connfd, buff, strlen(buff));
            struct sockaddr_in sa;
            socklen_t slen = sizeof sa;
            getpeername(connfd, (sockaddr *)&sa, &slen);
            std::cout << inet_ntop(AF_INET, &sa.sin_addr, buff, sizeof(buff)) << ": "<< ntohs(sa.sin_port)<< std::endl;
            close(connfd);
            exit(0);
        }
        close(connfd);
    }
    return 0;
}
