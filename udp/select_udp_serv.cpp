#include <iostream>
#include <arpa/inet.h> // AF_INET and sockaddr_in and socket
#include <unistd.h>    // include select
#include <string.h>    // bzero
#include <set>
#include <list>

#define SA(x) reinterpret_cast<sockaddr *>(x)

int main()
{
    int listenfd, connfd, udpfd, nready, maxfdp1;
    char msg[32];
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in cliaddr, servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    const int on = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    bind(listenfd, reinterpret_cast<sockaddr *>(&servaddr), sizeof(servaddr));
    listen(listenfd, 10);

    udpfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(udpfd, reinterpret_cast<sockaddr *>(&servaddr), sizeof(servaddr));
    fd_set rset, allset;
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);
    FD_SET(udpfd, &allset);
    maxfdp1 = std::max(listenfd, udpfd) + 1;
    socklen_t len = 0;
    std::set<int> s;
    socklen_t clilen = sizeof(cliaddr);
    std::list<int> clr;
    char buf[32];
    int n;
    while (true)
    {
        rset = allset;
        nready = select(maxfdp1 + 1, &rset, NULL, NULL, NULL);
        std::cout << "SELECT!!" << std::endl;
        if (FD_ISSET(listenfd, &rset))
        {
            clilen = sizeof cliaddr;
            connfd = accept(listenfd, (sockaddr *)&cliaddr, &clilen);
            s.insert(connfd);
            maxfdp1 = std::max(maxfdp1, connfd);
            FD_SET(connfd, &allset);
            if (--nready <= 0)
                continue;
        }

        if (FD_ISSET(udpfd, &rset))
        {
            clilen = sizeof cliaddr;
            n = recvfrom(udpfd, msg, 32, 0, SA(&cliaddr), &clilen);
            sendto(udpfd, msg, n, 0, SA(&cliaddr), clilen);
            std::cout << "udp recv : " << msg << std::endl;
            if (--nready <= 0)
                continue;
        }

        clr.clear();
        int len;

        for (const auto sockfd : s)
        {
            if (FD_ISSET(sockfd, &rset))
            {
                if ((n = read(sockfd, buf, 32)) > 0)
                {
                    for (auto tempfd : s)
                    {
                        // if(tempfd!=sockfd)
                        len = write(tempfd, buf, 32);
                    }
                    std::cout << len << std::endl;
                }
                if (n < 0)
                {
                    std::cerr << "read error" << std::endl;
                    exit(1);
                }
                if (n == 0)
                {
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    clr.insert(clr.end(), sockfd);
                }
                if (--nready <= 0)
                    break;
            }
        }
        for (auto clrfd : clr)
        {
            auto clrit = s.find(clrfd);
            s.erase(clrit);
        }
    }
}