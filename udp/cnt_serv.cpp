#include <iostream>
#include <arpa/inet.h> // AF_INET and sockaddr_in and socket

#include <string.h> // bzero
#include <thread>
#include <chrono>

static void recvfrom_int(int signo);
static int count = 0;

void dg_echo(int sockfd, sockaddr *pcliaddr, socklen_t clilen)
{
    int n;
    socklen_t len;
    char msg[2048];
    signal(SIGINT, recvfrom_int);
    n = 21000 * 1400;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &n, sizeof(n));
    while (true)
    {
        len = clilen;
        n = recvfrom(sockfd, msg, 2048, 0, pcliaddr, &len);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        count++;
    }
}

int main()
{
    int sockfd;
    sockaddr_in servaddr, cliaddr;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(sockfd, reinterpret_cast<sockaddr *>(&servaddr), sizeof(servaddr));
    dg_echo(sockfd, reinterpret_cast<sockaddr *>(&cliaddr), sizeof(cliaddr));
}

static void recvfrom_int(int signo) {
    std::cout << "received " << count << " datagrams" << std::endl;
    exit(0);
}