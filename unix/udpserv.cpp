#include <sys/un.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <unistd.h>

const static std::string UNIXDG_PATH = "/tmp/unix.dg";

void dg_echo(int sockfd, sockaddr *pcliaddr, socklen_t clilen)
{
    int n;
    socklen_t len;
    char msg[32];
    while (true)
    {
        len = clilen;
        n = recvfrom(sockfd, msg, 32, 0, pcliaddr, &len);
        std::cout << msg;
        n = sendto(sockfd, msg, 32, 0, pcliaddr, len);
        if (n < 0)
        {
            std::cerr << "sending error" << std::endl;
            exit(errno);
        }
    }
}

int main()
{
    int sockfd;
    sockaddr_un cliaddr, servaddr;
    sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    unlink(UNIXDG_PATH.c_str());
    memset(&servaddr, 0, sizeof(sockaddr_un));
    servaddr.sun_family = AF_LOCAL;
    strcpy(servaddr.sun_path, UNIXDG_PATH.c_str());
    bind(sockfd, reinterpret_cast<sockaddr *>(&servaddr), sizeof(sockaddr_un));
    dg_echo(sockfd, reinterpret_cast<sockaddr *>(&cliaddr), sizeof(sockaddr_un));
}