#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

int readable_timeo(int fd, int sec) {
    fd_set rset;
    timeval tv;
    FD_ZERO(&rset);
    FD_SET(fd, &rset);
    tv.tv_sec = sec;
    tv.tv_usec = 0;
    return select(fd + 1, &rset, NULL, NULL ,&tv);
}

void dg_cli(FILE *fp, int sockfd, const sockaddr* pservaddr, socklen_t servlen) {
    int n;
    char sendline[32], recvline[32];
    while(fgets(sendline, 32, fp) != NULL) {
        sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
        if(readable_timeo(sockfd, 5) == 0) {
            std::cout << "socket timeout" << std::endl;
        } else {
            n = recvfrom(sockfd, recvline, 32, 0, NULL, NULL);
            recvline[n] = 0;
            fputs(recvline, stdout);
        }
    }
}

int main()
{
    int sockfd;
    sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_port = htons(8888);
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    dg_cli(stdin, sockfd, reinterpret_cast<sockaddr*>(&servaddr), sizeof(servaddr));
}