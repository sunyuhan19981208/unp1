#include <iostream>
#include <arpa/inet.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>

const std::string UNIXSTR_PATH = "/tmp/deliver_fd.str";

static bool take_over_flag = false;

ssize_t write_fd(int fd, void *ptr, size_t nbytes, int sendfd)
{
    msghdr msg;
    iovec iov[1];
    union
    {
        cmsghdr cm;
        char control[CMSG_SPACE(sizeof(int))];
    } control_un;
    cmsghdr *cmptr;
    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);

    cmptr = CMSG_FIRSTHDR(&msg);
    cmptr->cmsg_len = CMSG_LEN(sizeof(int));
    cmptr->cmsg_level = SOL_SOCKET;
    cmptr->cmsg_type = SCM_RIGHTS;
    *((int *)CMSG_DATA(cmptr)) = sendfd;

    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    iov[0].iov_base = ptr;
    iov[0].iov_len = nbytes;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    return sendmsg(fd, &msg, 0);
}

void give_io()
{
    int sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    sockaddr_un servaddr;
    memset(&servaddr, 0, sizeof(sockaddr_un));
    servaddr.sun_family = AF_LOCAL;
    strcpy(servaddr.sun_path, UNIXSTR_PATH.c_str());
    if (connect(sockfd, reinterpret_cast<sockaddr *>(&servaddr), sizeof(sockaddr_un)) == -1)
    {
        std::cout << "NOT TAKE YET" << std::endl;
        return;
    }
    else
    {
        std::cout << "TAKE NOW" << std::endl;
        take_over_flag = true;
    }
    int fdin = fileno(stdin);
    int fdout = fileno(stdout);
    char temp[4];
    write_fd(sockfd, temp, 4, fdin);
    write_fd(sockfd, temp, 4, fdout);
    close(sockfd);
}

int main()
{
    // give_io();
    while (!take_over_flag)
    {
        int a, b;
        std::cin >> a >> b;
        std::cout << a + b << std::endl;
        if(!take_over_flag)give_io();
    }
}