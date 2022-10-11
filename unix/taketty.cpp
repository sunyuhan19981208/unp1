#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/un.h>

const std::string UNIXSTR_PATH = "/tmp/deliver_fd.str";

ssize_t read_fd(int fd, void *ptr, ssize_t nbytes, int *recvfd)
{
    msghdr msg;
    iovec iov[1];
    ssize_t n;
    union
    {
        cmsghdr cm;
        char control[CMSG_SPACE(sizeof(int))];
    } control_un;
    cmsghdr *cmptr;
    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    iov[0].iov_base = ptr;
    iov[0].iov_len = nbytes;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    if ((n = recvmsg(fd, &msg, 0)) <= 0)
        return n;
    cmptr = CMSG_FIRSTHDR(&msg);
    *recvfd = *((int *)CMSG_DATA(cmptr));
    return n;
}

void take_io(int &fdin, int &fdout)
{
    int listenfd, connfd;
    socklen_t clilen;
    sockaddr_un cliaddr, servaddr;
    listenfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    unlink(UNIXSTR_PATH.c_str());
    memset(&servaddr, 0, sizeof(sockaddr_un));
    servaddr.sun_family = AF_LOCAL;
    strcpy(servaddr.sun_path, UNIXSTR_PATH.c_str());
    bind(listenfd, reinterpret_cast<sockaddr *>(&servaddr), sizeof(sockaddr_un));
    listen(listenfd, 10);
    clilen = sizeof(cliaddr);
    connfd = accept(listenfd, reinterpret_cast<sockaddr *>(&cliaddr), &clilen);
    char temp[4];
    if (read_fd(connfd, temp, 4, &fdin) <= 0)
        std::cerr << "read fdin error";
    if (read_fd(connfd, temp, 4, &fdout) <= 0)
        std::cerr << "read fdin error";
    close(connfd);
    close(listenfd);
}

int get_from_user(int fd, char *buffer, size_t size)
{
    size_t cnt = 0;
    char c;

    if(buffer == NULL || size == 0)
        return 0;

    while(read(fd, &c, 1) == 1 && cnt < size - 1)
    {
        std::cout << c << std::endl;
        if(c == '\n')
        {
            buffer[cnt] = 0;
            return 1;
        }

        buffer[cnt++] = c;
    }

    buffer[cnt] = 0; // making sure it's 0-terminated
    return 1;
}

int main()
{
    int fdin, fdout;
    take_io(fdin, fdout);
    while (1)
    {
        int a, b;
        char buf[32];
        memset(buf, 0, 32);
        get_from_user(fdin, buf, 32);
        a = atoi(buf);
        std::cout <<a << std::endl;
        memset(buf, 0, 32);
        get_from_user(fdin, buf, 32);
        b = atoi(buf);

        std::cout <<b << std::endl;
        int res = a * b;
        memset(buf, 0, 32);
        std::string str = std::to_string(res);
        memcpy(buf, str.c_str(), str.length());
        write(fdout, buf, 32);
    }
}