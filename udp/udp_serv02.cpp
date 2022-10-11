#include <iostream>
#include <arpa/inet.h> // AF_INET and sockaddr_in and socket

#include <string.h> // bzero

void dg_echo(int sockfd, sockaddr *pcliaddr, socklen_t clilen)
{
    int n;
    socklen_t len;
    char msg[32];
    while (true)
    {
        len = clilen;
        n = recvfrom(sockfd, msg, 32, 0, pcliaddr, &len);
        char str[len];
        inet_ntop(AF_INET, &reinterpret_cast<sockaddr_in *>(pcliaddr)->sin_addr.s_addr,str, len);
        std::cout
            << str
            << std::endl;
        std::cout << ntohs(reinterpret_cast<sockaddr_in *>(pcliaddr)->sin_port) << std::endl;
        std::cout << msg;
        sendto(sockfd, msg, n, 0, pcliaddr, len);
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