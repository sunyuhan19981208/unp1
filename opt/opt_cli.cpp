#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

void str_cli(std::istream& is, int fd) {
    std::string s;
    char sendline[32];
    char* recvline = new char[32];
    while(is >> sendline) {
        int n;
        if((n = write(fd, sendline, 32)) < 0)
            std::cerr << "write error";
        if((n = read(fd, recvline, 32)) < 0)
            std::cerr << "read error";
        std::cout << recvline<< std::endl;
    }
}

int main() {
    int sockfd;
    sockaddr_in servaddr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int rcvbuf_len = 32;
    socklen_t opt_len = sizeof(rcvbuf_len);
    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcvbuf_len, opt_len);
    getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcvbuf_len, &opt_len);
    std::cout << rcvbuf_len << std::endl;
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr.s_addr);
    
    connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr));

    str_cli(std::cin, sockfd);
}