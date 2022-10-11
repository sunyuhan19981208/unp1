#include <arpa/inet.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/errno.h>
#include <iostream>
#include <string.h>
#include <sys/time.h>

const int MAXLINE = 1024 * 256;

char *gf_time(void)
{
    struct timeval tv;
    time_t t;
    static char str[30];
    char *ptr;
    if (gettimeofday(&tv, NULL) < 0)
    {
        std::cerr << "gettimeofday error" << std::endl;
        exit(1);
    }
    t = tv.tv_sec; /* POSIX says tv.tv_sec is time_t; some BSDs don't agree. */
    ptr = ctime(&t);
    strcpy(str, &ptr[11]);
    /* Fri Sep 13 00:00:00 1986\n\0 */
    /* 0123456789012345678901234 5 */
    snprintf(str + 8, sizeof(str) - 8, ".%06d", tv.tv_usec);
    return (str);
}

void str_cli(int sockfd)
{
    int maxfdp1, val, stdineof;
    char to[MAXLINE], fr[MAXLINE];
    char *toiptr, *tooptr, *friptr, *froptr;
    val = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, val | O_NONBLOCK);

    val = fcntl(fileno(stdin), F_GETFL, 0);
    fcntl(sockfd, F_SETFL, val | O_NONBLOCK);

    val = fcntl(fileno(stdout), F_GETFL, 0);
    fcntl(sockfd, F_SETFL, val | O_NONBLOCK);

    toiptr = tooptr = to;
    friptr = froptr = fr;
    stdineof = 0;

    fd_set rset, wset;
    ssize_t n, nwritten;
    maxfdp1 = std::max(sockfd, std::max(fileno(stdin), fileno(stdout))) + 1;
    while (1)
    {
        FD_ZERO(&rset);
        FD_ZERO(&wset);
        if (0 == stdineof && toiptr < &to[MAXLINE])
        {
            FD_SET(fileno(stdin), &rset);
        }
        if (friptr < &fr[MAXLINE])
        {
            FD_SET(sockfd, &rset);
        }
        if (tooptr != toiptr)
        {
            FD_SET(sockfd, &wset);
        }
        if (froptr != friptr)
        {
            FD_SET(fileno(stdout), &wset);
        }

        select(maxfdp1, &rset, &wset, NULL, NULL);
        /**
         * @brief read from stdin
         *
         */
        if (FD_ISSET(fileno(stdin), &rset))
        {
            if ((n = read(fileno(stdin), toiptr, &to[MAXLINE] - toiptr)) < 0)
            {
                if (errno != EWOULDBLOCK)
                {
                    std::cerr << gf_time() << ": read error on stdin" << std::endl;
                    exit(errno);
                }
            }
            else if (0 == n)
            {
                std::cerr << gf_time() << ": EOF on stdin" << std::endl;
                if (tooptr == toiptr)
                    shutdown(sockfd, SHUT_WR);
            }
            else
            {
                std::cerr << gf_time() << ": read " << n << " bytes from stdin" << std::endl;
                toiptr += n;
                FD_SET(sockfd, &wset);
            }
        }

        /**
         * @brief read from sockfd
         *
         */
        if (FD_ISSET(sockfd, &rset))
        {
            if ((n = read(sockfd, friptr, &fr[MAXLINE] - friptr)) < 0)
            {
                if (errno != EWOULDBLOCK)
                {
                    std::cerr << gf_time() << ": read error on sockfd" << std::endl;
                    exit(errno);
                }
            }
            else if (0 == n)
            {
                std::cerr << gf_time() << ": EOF on sockfd" << std::endl;
                if (stdineof == 1)
                {
                    return;
                }
                else
                {
                    std::cerr << gf_time() << ": server terminated" << std::endl;
                    exit(0);
                }
            }
            else
            {
                std::cerr << gf_time() << ": read " << n << " bytes from sockfd" << std::endl;
                friptr += n;
                FD_SET(fileno(stdout), &wset);
            }
        }

        /**
         * @brief write to stdout
         *
         */
        if (FD_ISSET(fileno(stdout), &wset) && (n = friptr - froptr) > 0)
        {
            if ((nwritten = write(fileno(stdout), froptr, n)) < 0)
            {
                if (errno != EWOULDBLOCK)
                {
                    std::cerr << gf_time() << ": write error on stdout" << std::endl;
                    exit(errno);
                }
            }
            else
            {
                std::cerr << gf_time() << ": wrote " << nwritten << " bytes to stdout" << std::endl;
                froptr += nwritten;
                if (froptr == friptr)
                {
                    friptr = froptr = fr;
                }
            }
        }

        /**
         * @brief write to sockfd
         *
         */
        if (FD_ISSET(sockfd, &wset) && (n = toiptr - tooptr) > 0)
        {
            if ((nwritten = write(sockfd, tooptr, n)) < 0)
            {
                if (errno != EWOULDBLOCK)
                {
                    std::cerr << gf_time() << ": write error on sockfd" << std::endl;
                    exit(errno);
                }
            }
            else
            {
                std::cerr << gf_time() << ": wrote " << nwritten << " bytes to sockfd" << std::endl;
                tooptr += nwritten;
                if (tooptr == toiptr)
                {
                    toiptr = tooptr = to;
                    if (stdineof)
                        shutdown(sockfd, SHUT_WR);
                }
            }
        }
    }
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

    if (connect(sockfd, (sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        std::cerr << "connect error" << std::endl;
        exit(1);
    }
    str_cli(sockfd);
}