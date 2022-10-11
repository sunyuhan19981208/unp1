#include <arpa/inet.h>
#include <signal.h>
#include <iostream>
#include <unistd.h> // include alarm 

static void connect_alarm(int);

int connect_timeo(int sockfd, const sockaddr * saptr, socklen_t salen, int nsec) {
    auto sigfunc = signal(SIGALRM, connect_alarm);
    if(alarm(nsec)!=0){
        std::cerr << "connect timeo: alarm already set" << std::endl;
        exit(1);
    }
    int n;
    if((n = connect(sockfd, saptr, salen)) < 0) {
        close(sockfd);
        if(errno ==  EINTR) {
            errno = ETIMEDOUT;
        }
    }
    alarm(0);
    signal(SIGALRM, sigfunc);
    return n;
}
static void connect_alarm(int sig) {
    return;
}