#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"
#include <sys/wait.h>
#include <string.h>


using namespace std;

void ctrlZHandler(int sig_num) {
    cout << "smash: got ctrl-Z" << endl;
    auto& smashy = SmallShell::getInstance();
    pid_t foregroundChildPID = smashy.getForegroundCommandPID();
    if (foregroundChildPID == 0){
        return;
    }
    smashy.updateForegroundCommandPID(0);
    cout <<"smash: process "<< foregroundChildPID<<" was stopped"<< endl;
    kill(foregroundChildPID, SIGSTOP);

    //preperaing the cmd_line
    string temp = smashy.get_fg_cmd_line();
    const char* cmd_line = (temp).c_str();

    //yolo

    JobsList* jobList = smashy.getJoblist();
    if(jobList->getJobByPID(foregroundChildPID) == nullptr){
        jobList->addJob(cmd_line, foregroundChildPID, true); //usual suspect !!
    }
    else{
        jobList->resetJobTime(foregroundChildPID);
    }

    //clean up
    smashy.updateForegroundCommandPID(0);
    smashy.update_fg_cmd_line("");
}

void ctrlCHandler(int sig_num) {
    cout << "smash: got ctrl-C" << endl;
    auto& smashy = SmallShell::getInstance();
    pid_t foregroundChildPID = smashy.getForegroundCommandPID();
    if (foregroundChildPID == 0){
        return;
    }
    kill(foregroundChildPID, SIGKILL);
    cout << "smash: process "<< foregroundChildPID <<" was killed" << endl;

    //clean up
    JobsList* jobList = smashy.getJoblist();
    jobList->removeJobByPID(foregroundChildPID);
    smashy.updateForegroundCommandPID(0);
    smashy.update_fg_cmd_line("");
//    wait(nullptr);
}

void alarmHandler(int sig_num) {
    cout << "got an alarm" << endl;
    auto& smashy = SmallShell::getInstance();
    smashy.reap();


    time_t currentTime = time(nullptr);
    double difference;
    int flag;
//    JobsList* jobList = smashy.getJoblist();

    //delete timeout entries
    int i = 0;
    for (TimeoutEntry* entry : smashy.timeoutEntryVector){
        difference = difftime(currentTime, *(entry->timeEntry));
        if( difference  >= entry->timeToRun){
            flag = kill(entry->pid, SIGKILL);
            if (flag == -1){
                perror("smash error: kill failed");
            }
            cout << "smash: " <<  entry->cmdString << " timed out!" << endl;
            smashy.timeoutEntryVector.erase(smashy.timeoutEntryVector.begin() + i);
            if( entry->pid == smashy.getForegroundCommandPID()){  //if it was a foreground command that timedout
                smashy.updateForegroundCommandPID(0);
                smashy.update_fg_cmd_line("");
            }
        }
        ++i;
    }
}
