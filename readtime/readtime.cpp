#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

const int MAX_LINE = 8;
/**
 * @brief 从远程服务器读取时间
 *
 * @param argc
 * @param argv 目标地址
 * @return int
 */
int main(int argc, char **argv)
{
    int sockfd, n;
    char recvline[MAX_LINE + 1];
    struct sockaddr_in servaddr;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0)
    {
        std::cerr << "socket error";
        exit(errno);
    }
    bzero(&servaddr, sizeof servaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
    {
        std::cerr << "inet_pton errot for " << argv[1];
        exit(errno);
    }
    if (connect(sockfd, (struct sockaddr *)(&servaddr), sizeof(servaddr)) < 0)
    {
        std::cerr << "connect error";
        exit(errno);
    }
    int cnt =0;
    std::string res;
    while ((n = read(sockfd, recvline, MAX_LINE)) > 0)
    {
        cnt++;
        recvline[n] = 0;
        res += recvline;
    }
    if (n < 0)
    {
        std::cerr << "read error";
        exit(errno);
    }
    else
    {
        std::cout << res;
        std::cout<<cnt << std::endl;
    }
}