#include <arpa/inet.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>
#include <string.h>

const std::string UNIXSTR_PATH = "/tmp/unix.str";

void str_echo(int fd) {
    ssize_t n;
    char buf[32];
    while((n = read(fd, buf, 32))>0) {
        int len = write(fd, buf, 32);
    }
    if(n < 0) {
        std::cerr << "read error" << std::endl;
        exit(1);
    }
    std::cout <<"end lo" << std::endl;
}
/**
 * @brief tcp server in unix domain
 * 
 * @return int 
 */
int main() {
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    sockaddr_un cliaddr, servaddr;
    void sig_chld(int);
    listenfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    unlink(UNIXSTR_PATH.c_str());
    memset(&servaddr, 0, sizeof(sockaddr_un));
    servaddr.sun_family = AF_LOCAL;
    strcpy(servaddr.sun_path, UNIXSTR_PATH.c_str());
    bind(listenfd, reinterpret_cast<sockaddr*>(&servaddr), sizeof(sockaddr_un));
    listen(listenfd, 10);
    signal(SIGCHLD, sig_chld);
    while(1) {
        clilen = sizeof(cliaddr);
        if((connfd = accept(listenfd, reinterpret_cast<sockaddr*>(&cliaddr), &clilen)) < 0) {
            if(errno == EINTR)
                continue;
            else {
                std::cerr << "accept error" << std::endl;
                exit(1);
            }
        }
        if((childpid=fork())==0) {
            close(listenfd);
            str_echo(connfd);
            exit(0);
        }
        close(connfd);
    }
}
/**
 * @brief this function is not good enough
 * 
 */
// void sig_chld(int signo) {
//     pid_t pid;
//     int stat;
//     pid = wait(&stat);
//     std::cout << "child " <<  pid << " terminated" << std::endl;
// }
void sig_chld(int signo) {
    pid_t pid;
    int stat;
    while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        std::cout << "child " <<  pid << " terminated" << std::endl;
    return;
}