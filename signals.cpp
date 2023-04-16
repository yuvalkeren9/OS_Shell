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
    smashy.getJoblist()->addJob(smashy.getExternalCommandInFgPointer(), foregroundChildPID, true);
}

void ctrlCHandler(int sig_num) {
    auto& smashy = SmallShell::getInstance();
    pid_t foregroundChildPID = smashy.getForegroundCommandPID();
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
    cout << "a child has sent SIGCHLD" <<endl;
    if (smashy.getForegroundCommandPID() != 0){
        return;
    }
    else{
        wait(nullptr);
    }
}
