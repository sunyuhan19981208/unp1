#include <unistd.h>
#include <iostream>
#include <signal.h>

void handle_alarm(int sig){
    std::cout << "ALARM!" << std::endl;
}
int main() {
    signal(SIGALRM, handle_alarm);
    alarm(5);
    sleep(100);
    std::cout << "FINISH SLEEP" << std::endl;
}