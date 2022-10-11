#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/errno.h>
#include <iostream>
#include <string.h>

int connect_nonb(int sockfd, const sockaddr *saptr, socklen_t salen, int nsec)
{
    int flags, n, error;
    socklen_t len;
    fd_set rset, wset;
    timeval tval = {
        .tv_sec = nsec,
        .tv_usec = 0,
    };
    flags = fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    error = 0;
    if ((n = connect(sockfd, saptr, salen)) < 0)
        if (errno != EINPROGRESS)
            return -1;
    /* Do whatever we want while the connect is taking place. */
    if (n == 0)
        goto done;
    if ((n = select(sockfd + 1, &rset, &wset, NULL, nsec ? &tval : NULL)) == 0)
    {
        close(sockfd);
        errno = ETIMEDOUT;
        return -1;
    }
    if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset))
    {
        len = sizeof error;
        if ((getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0))
            return -1;
    }
    else
    {
        std::cerr << "select error: sockfd not set" << std::endl;
    }
done:
    fcntl(sockfd, F_SETFL, flags);
    if (error)
    {
        close(sockfd);
        errno = error;
        return -1;
    }
    return 0;
}

int main()
{
    int sockfd;
    sockaddr_in servaddr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr.s_addr);

    if (connect_nonb(sockfd, (sockaddr *)&servaddr, sizeof(servaddr), 5) < 0)
    {
        if (errno == ETIMEDOUT)
        {
            std::cerr << "timeout error" << std::endl;
        }
        std::cerr << "connect error" << std::endl;
        exit(1);
    }
}