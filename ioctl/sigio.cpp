#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

void sigio_handler(int signo) {
    std::cout << "SIGIO" << std::endl;
}

int main() {
    signal(SIGIO, sigio_handler);
    int fd = open("/Users/sunyuhan/Code/cpp/unp1/ioctl/sig", O_RDONLY);
    fcntl(fd, F_SETOWN, getpid());
    int32_t oflags = fcntl(fd, F_GETFL);
    int32_t ret = fcntl(fd, F_SETFL, oflags | FASYNC);
    while(1);
}