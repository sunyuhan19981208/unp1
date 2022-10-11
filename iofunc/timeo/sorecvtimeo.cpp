#include <arpa/inet.h>
#include <iostream>
#include <unistd.h> // include alarm 
#include <string.h>

void dg_cli(FILE *fp, int sockfd, const sockaddr* pservaddr, socklen_t servlen) {
    int n;
    char sendline[32], recvline[32];
    timeval tv = {
        .tv_sec = 5,
        .tv_usec = 0,
    };
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    while(fgets(sendline, 32, fp) != NULL) {
        sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
        n = recvfrom(sockfd, recvline, 32, 0, NULL, NULL);
        if(n < 0) {
            if(errno == EWOULDBLOCK)
                std::cerr << "socket timeout" << std::endl;
            else {
                std::cerr << "recv error" << std::endl;
                exit(errno);
            }
        } else {
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

static void sig_alarm(int sig) {
}