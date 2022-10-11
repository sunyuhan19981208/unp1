#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <poll.h>
#include <limits.h>

void solve_sigpipe(int sig)
{
    std::cout << "SIGPIPE" << std::endl;
}

int main()
{
    signal(SIGPIPE, solve_sigpipe);
    int listenfd, connfd;
    pid_t childpid;
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

    pollfd client[OPEN_MAX]; 
    client[0].fd = listenfd;
    client[0].events = POLLRDNORM;
    for(int i=1;i<OPEN_MAX;i++)
        client[i].fd = -1;
    int maxi = 0;

    int nready;

    ssize_t n;
    char buf[32];

    /**
     * @brief temp fd variable
     * 
     */
    int sockfd;

    while (true)
    {
        nready = poll(client, maxi + 1, -1);
        if (client[0].revents & POLLRDNORM)
        {
            clilen = sizeof cliaddr;
            connfd = accept(listenfd, (sockaddr *)&cliaddr, &clilen);
            int i;
            for(i=1;i<OPEN_MAX;i++)
                if(client[i].fd < 0){
                    client[i].fd = connfd;
                    break;
                }
            if(OPEN_MAX == i){
                std::cerr << "TOO MANY CLIENTS" << std::endl;
                exit(1);
            }

            client[i].events = POLLRDNORM;
            if(i > maxi){
                maxi = i;
            }
            if (--nready <= 0)
                continue;
        }

        for (int i = 1;i<=maxi;i++)
        {
            if((sockfd = client[i].fd)<0)
                continue;
            if (client[i].revents & (POLLRDNORM | POLLERR))
            {
                if ((n = read(sockfd, buf, 32)) > 0)
                {
                    write(sockfd, buf, n);
                }
                if (n < 0)
                {
                    if(ECONNRESET == errno){
                        close(sockfd);
                        client[i].fd = -1;                        
                    } else {
                        std::cerr << "read error" << std::endl;
                        exit(1);
                    }
                }
                if(n == 0){
                    close(sockfd);
                    client[i].fd = -1;
                }
                if(--nready <= 0)
                    break;
            }
        }
    }
}