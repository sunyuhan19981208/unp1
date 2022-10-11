#include <arpa/inet.h>
#include <signal.h>
#include <iostream>
#include <unistd.h> // include alarm 
#include <string.h>

static void sig_alarm(int);

void dg_cli(FILE *fp, int sockfd, const sockaddr* pservaddr, socklen_t servlen) {
    int n;
    char sendline[32], recvline[32];
    struct sigaction sact = {
        .sa_handler = sig_alarm,
        .sa_flags = 0,
    };
    sigaction(SIGALRM, &sact, NULL);
    while(fgets(sendline, 32, fp) != NULL) {
        sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
        alarm(5);
        n = recvfrom(sockfd, recvline, 32, 0, NULL, NULL);
        if(n < 0) {
            if(errno == EINTR)
                std::cerr << "socket timeout" << std::endl;
            else {
                std::cerr << "recv error" << std::endl;
                exit(errno);
            }
        } else {
            alarm(0);
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