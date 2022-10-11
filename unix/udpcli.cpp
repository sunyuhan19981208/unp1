#include <iostream>
#include <sys/un.h>
#include <string.h>
#include <arpa/inet.h>

const static std::string UNIXDG_PATH = "/tmp/unix.dg";

void dg_cli(FILE *fp, int sockfd, const sockaddr *pservaddr, socklen_t servlen)
{
    int n;
    char sendline[32], recvline[32];
    while(fgets(sendline, 32, fp)!=NULL) {
        int k = sendto(sockfd, sendline, 32, 0, pservaddr, servlen);
        if(k == -1){
            std::cerr << "sending error" << std::endl;
            exit(1);
        }
        int n = recvfrom(sockfd, recvline, 32, 0, NULL, NULL);
        std::cout << recvline;
    }
}

int main() {
    int sockfd;
    sockaddr_un cliaddr, servaddr;
    sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    memset(&cliaddr, 0, sizeof(sockaddr_un));
    cliaddr.sun_family = AF_LOCAL;
    strcpy(cliaddr.sun_path, tmpnam(NULL));
    bind(sockfd, reinterpret_cast<sockaddr*>(&cliaddr), sizeof(cliaddr));
    memset(&servaddr, 0, sizeof(sockaddr_un));
    servaddr.sun_family = AF_LOCAL;
    strcpy(servaddr.sun_path, UNIXDG_PATH.c_str());
    dg_cli(stdin, sockfd, reinterpret_cast<sockaddr*>(&servaddr), sizeof(servaddr));
}