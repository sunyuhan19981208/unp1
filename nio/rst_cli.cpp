#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

int main() {
    int sockfd;
    linger ling;
    sockaddr_in servaddr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
    int n = connect(sockfd, reinterpret_cast<sockaddr*>(&servaddr), sizeof(sockaddr));
    if(n == -1) {
        std::cerr << "connect failed" << std::endl; 
        exit(0);
    }
    ling.l_onoff = 1;
    ling.l_linger = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));
    close(sockfd);
}