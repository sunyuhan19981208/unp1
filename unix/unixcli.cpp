#include <sys/un.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <unistd.h>

const std::string UNIXSTR_PATH = "/tmp/unix.str";

void str_cli(std::istream& is, int fd) {
    std::string s;
    char sendline[32];
    char* recvline = new char[32];
    while(is.getline(sendline, 32)) {
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
    sockaddr_un servaddr;
    sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    memset(&servaddr, 0, sizeof(sockaddr_un));
    servaddr.sun_family = AF_LOCAL;
    strcpy(servaddr.sun_path, UNIXSTR_PATH.c_str());
    connect(sockfd, reinterpret_cast<sockaddr*>(&servaddr), sizeof(sockaddr_un));
    str_cli(std::cin, sockfd);
}