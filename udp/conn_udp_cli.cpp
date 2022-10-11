#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <unistd.h>

void dg_cli(FILE *fp, int sockfd, const sockaddr *pservaddr, socklen_t servlen)
{
    int n;
    char sendline[32], recvline[32];
    sockaddr_in cliaddr;
    if(connect(sockfd, pservaddr, servlen) < 0){
        std::cerr << "connecting error" << std::endl;
        exit(1);
    }
    socklen_t clilen;
    clilen = sizeof cliaddr;
    getsockname(sockfd, reinterpret_cast<sockaddr*>(&cliaddr), &clilen);
    char str[20];
    inet_ntop(AF_INET, &reinterpret_cast<sockaddr_in *>(&cliaddr)->sin_addr.s_addr,str, sizeof(cliaddr));
    std::cout << str << " : " << ntohs(cliaddr.sin_port) << std::endl;
    while(fgets(sendline, 32, fp)!=NULL) {
        int k = write(sockfd, sendline, 32);
        std::cout << k << std::endl;
        if(k == -1){
            std::cerr << "write error" << std::endl;
            exit(1);
        }
        int n = read(sockfd, recvline, 32);
        if(n == -1){
            std::cerr << "read error" << std::endl;
        }
        std::cout << recvline;
    }
}
/**
 * @brief connected udp client
 * 
 * @return int 
 */
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