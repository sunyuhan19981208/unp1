#include <arpa/inet.h>
#include <string.h>
#include <iostream>

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
/**
 * @brief udpcli02
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