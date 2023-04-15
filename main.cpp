#include <iostream>
#include <unistd.h>


using std::endl;
using std::cout;

int main(int argc, char* argv[]) {
//    if(signal(SIGTSTP , ctrlZHandler)==SIG_ERR) {
//        perror("smash error: failed to set ctrl-Z handler");
//        printf("Meow the cat said");
//    }
//    if(signal(SIGINT , ctrlCHandler)==SIG_ERR) {
//        perror("smash error: failed to set ctrl-C handler");
//    }
//    signal(SIGCHLD, sigChildHandler);
//
//    //TODO: setup sig alarm handler
//
//    SmallShell& smash = SmallShell::getInstance();
//
//    while(true) {
////        std::cout << "smash> ";
//        std::cout << smash.getPromt();
//        std::string cmd_line;
//        std::getline(std::cin, cmd_line);
//        smash.executeCommand(cmd_line.c_str());
//    }
//    return 0;
    std::cout << "started sleeping" << endl;
    sleep(5);
    cout << "stopped sleeping, exiting" << endl;

}