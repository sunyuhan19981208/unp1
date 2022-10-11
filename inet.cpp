#include <iostream>
#include <arpa/inet.h>
/**
 * @brief inet_aton and inet_ntoa are only used in IPv4
 * 
 */
void exp1();
/**
 * @brief inet_pton and inet_ntop are used in both IPv4 and IPv6
 * 
 */
void exp2();
int main() {

}
void exp1() {
    char *s = "255.255.255.255";
    in_addr addr;
    inet_aton(s, &addr);
    std::cout << (void *)s << " : " << addr.s_addr << std::endl;
    char *res = inet_ntoa(addr);
    std::cout << (void *)res << " : " << res << std::endl;
}
void exp2() {
    
}