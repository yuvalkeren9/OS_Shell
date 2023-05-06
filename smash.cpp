#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "Commands.h"
#include "signals.h"

using std::endl;
using std::cout;

int main(int argc, char* argv[]) {
    if(signal(SIGTSTP , ctrlZHandler)==SIG_ERR) {
        perror("smash error: failed to set ctrl-Z handler");
    }
    if(signal(SIGINT , ctrlCHandler)==SIG_ERR) {
        perror("smash error: failed to set ctrl-C handler");
    }
    //TODO: setup sig alarm handler

//    sigaction(SIGALRM, alarmHandler );   //TODO: sigaction  TODO: like the above

    struct sigaction sa;
    sa.sa_handler = alarmHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("smash error: sigaction failed");
        exit(-1);
    }

    SmallShell& smash = SmallShell::getInstance();

    while(true) {
        std::cout << smash.getPromt();
        std::string cmd_line;
        std::getline(std::cin, cmd_line);
        smash.executeCommand(cmd_line.c_str());
    }

}