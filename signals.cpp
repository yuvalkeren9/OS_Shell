#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"
#include <sys/wait.h>
#include <string.h>


using namespace std;

void ctrlZHandler(int sig_num) {
    auto& smashy = SmallShell::getInstance();
    pid_t foregroundChildPID = smashy.getForegroundCommandPID();
    if (foregroundChildPID == 0){
        cout << "No child to stop.. moron" << endl; //TODO: CHECK THE CORRECT PRINT ASKED
        return;
    }
    smashy.updateForegroundCommandPID(0);
    cout <<"Process "<< foregroundChildPID<<" was stopped"<< endl;
    kill(foregroundChildPID, SIGSTOP);

    //preperaing the cmd_line
    string temp = smashy.get_fg_cmd_line();
    const char* cmd_line = (temp).c_str();

    //yolo

    JobsList* jobList = smashy.getJoblist();
    if(jobList->getJobByPID(foregroundChildPID) == nullptr){
        jobList->addJob(cmd_line, foregroundChildPID, true); //usual suspect !!
    return;
    }
    else{
        jobList->resetJobTime(foregroundChildPID);
    }
}

void ctrlCHandler(int sig_num) {
    auto& smashy = SmallShell::getInstance();
    pid_t foregroundChildPID = smashy.getForegroundCommandPID();
    if (foregroundChildPID == 0){
//        cout << "No child to kill.. moron" << endl;
        return;
    }
//    cout <<"Killing the child! sorry baby.." << endl;
    kill(foregroundChildPID, SIGKILL);
//    cout <<"The child is dead" << endl;
    wait(nullptr);
}

void alarmHandler(int sig_num) {
  // TODO: Add your implementation
}
