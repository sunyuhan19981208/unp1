#include <arpa/inet.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <string.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>

struct dvpoll {
    pollfd *dp_fds;
    int dp_nfds;
    int dp_timeout;
};

void str_cli(int fd) {
    int stdineof;
    char buf[32];
    int n, wfd;
    pollfd pfd[2];
    dvpoll dopoll;
    int i, res;
    wfd = open("/tmp/poll", O_RDWR, 0);
    pfd[0].fd = fileno(stdin);
    pfd[0].events = POLLIN;
    pfd[0].revents = 0;
    pfd[0].fd = fd;
    pfd[0].events = POLLIN;
    pfd[0].revents = 0;
    write(wfd, pfd, sizeof(pollfd)*2);
    while(1) {
        dopoll.dp_timeout = -1;
        dopoll.dp_fds = pfd;
        dopoll.dp_nfds = 2;
        res = ioctl(wfd, , &dopoll);

    }
}
/**
 * @brief this program can not run on macOs
 * 
 * @return int 
 */
int main() {
    int sockfd;
    sockaddr_in servaddr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr.s_addr);
    
    connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr));

    str_cli(sockfd);
}