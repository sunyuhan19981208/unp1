#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

int main() {
    int fd = open("/Users/sunyuhan/Code/cpp/unp1/ioctl/sig", O_WRONLY|O_APPEND);
    char buf[8] = "1234567";
    buf[7]='8';
    int n = write(fd,buf, 8);
    int pid = fcntl(fd, F_GETOWN, 0);
    std::cout << pid << std::endl;
    kill(pid, SIGIO);
    close(fd);
    std::cout << n << std::endl;
}