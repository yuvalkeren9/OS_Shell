#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"
#include <sys/wait.h>


using namespace std;

void ctrlZHandler(int sig_num) {
    auto& smashy = SmallShell::getInstance();
    pid_t foregroundChildPID = smashy.getForegroundCommandPID();
    if (foregroundChildPID == 0){
        cout << "No child to stop.. moron" << endl;
        return;
    }
    cout <<"Stopping the child!" << endl;
    kill(foregroundChildPID, SIGSTOP);
    smashy.getJoblist()->addJob("Anna zack", foregroundChildPID, true); //usual suspect !!
    //smashy.getExternalCommandInFgPointer()->getCommand()
}

void ctrlCHandler(int sig_num) {
    auto& smashy = SmallShell::getInstance();
    pid_t foregroundChildPID = smashy.getForegroundCommandPID();
    if (foregroundChildPID == 0){
        cout << "No child to kill.. moron" << endl;
        return;
    }
    cout <<"Killing the child! sorry baby.." << endl;
    kill(foregroundChildPID, SIGTERM);
    cout <<"The child is dead" << endl;
    wait(nullptr);
}

void alarmHandler(int sig_num) {
  // TODO: Add your implementation
}

void sigChildHandler(int sig_num){
    auto& smashy = SmallShell::getInstance();
    int status;

    cout << "a child has sent SIGCHLD" <<endl;
//    waitpid(-1, &status, WUNTRACED | WCONTINUED | WNOHANG);
//    if(WIFCONTINUED(status)){
//        printf("\n currentState = continued!\n");
//    }
//    if(WIFSIGNALED(status)){
//        printf("\n currentState = signaled!\n");
//    }
//    if(WIFEXITED(status)){
//        printf("\nterminattor\n");
//    }
//    if(WIFSTOPPED(status)){
//        printf("\n currentState = stopped!\n");
//    }
//    cout << "went past checks";
//    if (smashy.getForegroundCommandPID() != 0){
//        return;
//    }
//    else{
//        wait(nullptr);
//    }
}
