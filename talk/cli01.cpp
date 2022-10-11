#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

void str_cli(std::istream& is, int fd) {
    std::string s;
    char sendline[4], recvline[4];
    while(is >> sendline) {

        write(fd, sendline, strlen(sendline));
        int n;
        read(fd, recvline, 4);
        std::cout << recvline<< std::endl;
    }
}

int main() {
    int sockfd;
    sockaddr_in servaddr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr.s_addr);
    
    connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr));

    str_cli(std::cin, sockfd);
}