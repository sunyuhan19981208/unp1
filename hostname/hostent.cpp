#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>

int main() {
    auto hptr = gethostbyname("www.sunyuhan.com");
    if(hptr == nullptr) {
        std::cerr << "unknown host" << std::endl;
        exit(0);
    }
    char str[INET_ADDRSTRLEN];
    char** pptr;
    for(pptr = hptr->h_aliases;*pptr!=NULL;pptr++)
        std::cout << "alias : " << *pptr << std::endl;
    switch (hptr->h_addrtype)
    {
    case AF_INET:
        pptr = hptr->h_addr_list;
        for(;*pptr!=NULL;pptr++) {
            std::cout << inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)) << std::endl;
        }
        break;
    default:
        std::cerr << "unsupported type" << std::endl;
        break;
    }
}