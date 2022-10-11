#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <set>
#include <list>

const int MAXLINE = 1024 * 256;

void solve_sigpipe(int sig)
{
    std::cout << "SIGPIPE" << std::endl;
}

int main()
{
    signal(SIGPIPE, solve_sigpipe);
    int listenfd, connfd;
    socklen_t clilen;
    sockaddr_in cliaddr, servaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(8888);

    bind(listenfd, (sockaddr *)&servaddr, sizeof(servaddr));

    listen(listenfd, 10);

    int maxfd = listenfd;

    std::set<int> s{listenfd};
    fd_set rset, allset;
    clilen = sizeof(cliaddr);

    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    int nready;

    ssize_t n;
    char buf[MAXLINE];
        
    std::list<int> clr;

    while (true)
    {
        rset = allset;
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
        if (FD_ISSET(listenfd, &rset))
        {
            connfd = accept(listenfd, (sockaddr *)&cliaddr, &clilen);
            s.insert(connfd);
            maxfd = std::max(maxfd, connfd);
            FD_SET(connfd, &allset);
            if (--nready <= 0)
                continue;
        }

        clr.clear();
        int len;

        for (const auto sockfd : s)
        {
            if (FD_ISSET(sockfd, &rset))
            {
                if ((n = read(sockfd, buf, MAXLINE)) > 0)
                {
                    for(auto tempfd : s){
                        // if(tempfd!=sockfd)
                        len = write(tempfd, buf, n);
                    }
                    std::cout << len << std::endl;
                }
                if (n < 0)
                {
                    std::cerr << "read error" << std::endl;
                    exit(1);
                }
                if(n == 0){
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    clr.insert(clr.end(), sockfd);
                }
                if(--nready <= 0)
                    break;
            }
        }
        for(auto clrfd : clr){
            auto clrit = s.find(clrfd);
            s.erase(clrit);
        }
    }
}