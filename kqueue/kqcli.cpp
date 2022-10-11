#include <arpa/inet.h>
#include <string.h>
#include <sys/event.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>

void str_cli(int fd)
{
    int kq, n, stdineof = 0, isfile;
    char buf[32];
    struct kevent kev[2];
    struct timespec ts = {
        .tv_sec = 0,
        .tv_nsec = 0,
    };
    struct stat st;

    isfile = ((fstat(fileno(stdin), &st)) && (st.st_mode & S_IFMT) == S_IFREG);
    EV_SET(&kev[0], fileno(stdin), EVFILT_READ, EV_ADD, 0, 0, NULL);
    EV_SET(&kev[1], fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    kq = kqueue();
    kevent(kq, kev, 2, NULL,0, &ts);
    while(1) {
        // memset(buf, 0, sizeof(buf));
        int nev = kevent(kq, NULL, 0, kev, 2, NULL);
        for(int i=0;i<nev;i++) {
            if(kev[i].ident == fd) {
                if((n = read(fd, buf, 32)) == 0) { 
                    if(stdineof == 1)
                        return;
                    else {
                        std::cerr << "server terminated" << std::endl;
                        exit(1);
                    }
                }
                write(fileno(stdout), buf, 32);
            }
            if(kev[i].ident == fileno(stdin)) {
                n = read(fileno(stdin), buf, 32);
                if(n > 0) {
                    write(fd, buf, n);
                }
                if(n == 0 || (isfile && n == kev[i].data)) {
                    stdineof = 1;
                    shutdown(fd, SHUT_WR);
                    kev[i].flags = EV_DELETE;
                    kevent(kq, &kev[i], 1, NULL, 0, &ts);
                    continue;
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

    connect(sockfd, (sockaddr *)&servaddr, sizeof(servaddr));

    str_cli(sockfd);
}