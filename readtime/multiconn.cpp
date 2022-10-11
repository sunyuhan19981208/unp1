#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <future>
#include <functional>

const int MAX_LINE = 8;
/**
 * @brief 从远程服务器读取时间
 *
 * @param argc
 * @param argv 目标地址
 * @return int
 */
void conn(int i){
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
    if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0)
    {
        std::cerr << "inet_pton errot for 127.0.0.1";
        exit(errno);
    }
    if (connect(sockfd, (struct sockaddr *)(&servaddr), sizeof(servaddr)) < 0)
    {
        std::cerr << "connect error : "<< i << std::endl;
        // exit(errno);
    } else {
        std::cout << "connected : " << i << std::endl;
    }
    // std::this_thread::sleep_for(std::chrono::seconds(5));
}
int main()
{
    std::future<void> fut[100];
    for(int i=0;i<100;i++){
        fut[i] = std::async(conn, i);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

}