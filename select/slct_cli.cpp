#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

void str_cli(std::istream& is, int fd) {
    int maxfdp1;
    fd_set rset;
    FD_ZERO(&rset);
    char sendline[32];
    char* recvline = new char[32];
    while(1) {
        FD_SET(fileno(stdin), &rset);
        FD_SET(fd, &rset);
        maxfdp1 = std::max(fd, fileno(stdin)) + 1;
        select(maxfdp1, &rset, NULL, NULL, NULL);
        int n;
        if(FD_ISSET(fileno(stdin), &rset)) {
            is.getline(sendline, 32);
            if(is.good()){
                if((n = write(fd, sendline, 32)) < 0){
                    std::cerr << "write error";
                    exit(errno);
                }
            } else {
                shutdown(fd, SHUT_WR);
            }
        }
        if(FD_ISSET(fd, &rset)) {
            if((n = read(fd, recvline, 32)) == 0) {
                if(!is.good()){
                    std::cout << "client terminated" << std::endl;
                    exit(0);
                }
                std::cerr << "server terminated" << std::endl;
                exit(errno);
            }
            std::cout << recvline << std::endl;
        }
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