#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"
#include <sys/wait.h>


using namespace std;

void ctrlZHandler(int sig_num) {
    auto& smashy = SmallShell::getInstance();
    pid_t foregroundChildPID = smashy.getForegroundCommandPID();
    cout <<"Stopping the child!" << endl;
    kill(foregroundChildPID, SIGSTOP);
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
    wait(nullptr);
}
