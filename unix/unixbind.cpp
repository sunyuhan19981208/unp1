#include <arpa/inet.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

int main() {
    char *tmp = "/tmp/moose";
    int fd;
    sockaddr_un addr1, addr2;
    fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    unlink(tmp);
    bzero(&addr1, sizeof(sockaddr_un));
    addr1.sun_family = AF_LOCAL;
    strncpy(addr1.sun_path, tmp, sizeof(addr1.sun_path) - 1);
    bind(fd, reinterpret_cast<const sockaddr *>(&addr1), SUN_LEN(&addr1));
    socklen_t len = sizeof(addr2);
    getsockname(fd, reinterpret_cast<sockaddr *>(&addr2), &len);
    std::cout << "bound name = "<< addr2.sun_path << ", returned len = " << len << std::endl; 
}