#include <arpa/inet.h>
#include <string.h>
#include <iostream>

const int NDG = 20000;
const int DGLEN = 1400;

void dg_cli(FILE *fp, int sockfd, const sockaddr *pservaddr, socklen_t servlen)
{
    int i;
    char sendline[DGLEN];
    for (i = 0; i < NDG; i++)
    {
        int k = sendto(sockfd, sendline, DGLEN, 0, pservaddr, servlen);
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
    dg_cli(stdin, sockfd, reinterpret_cast<sockaddr *>(&servaddr), sizeof(servaddr));
}