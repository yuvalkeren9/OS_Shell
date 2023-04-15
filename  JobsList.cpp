//
// Created by student on 4/14/23.
//

#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"
#include <time.h>


using namespace std;

JobsList::JobEntry::JobEntry(int jobID, pid_t pid, const ExternalCommand& command , bool stopped): jobID(jobID), pid(pid), command(command),
stopped(stopped)
        {
//    time_t* temptime = new time_t;
//    jobTime = time(temptime);
    time(jobTime);
}
JobsList::JobEntry::~JobEntry(){
}

void JobsList::JobEntry::printJob(){
      string cmd_s = _trim(string(command.getCommand()));
    string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));
    cout<< "[" << jobID << "] "<< firstWord << " "<< pid << " "<< jobTime;
    if(stopped)
    {
        cout<< "(stopped)" ;
    }
    cout << endl;

}
 int JobsList::JobEntry::getJobID() const {
    return jobID;
}

JobsList::JobsList():numOfJobs(0) {
}

void JobsList::addJob(ExternalCommand *cmd,pid_t pid, bool isStopped) {
    int newJobID = getLargestJobID()+1;
   const char* newCommand = cmd->getCommand();
    JobEntry* newJob = new JobEntry(newJobID,pid,newCommand,isStopped);

}

int JobsList::getLargestJobID() {
    int max =0;
    for(const JobEntry *job:jobsVector){
        if(job->getJobID()> max)
        {
            max=job->getJobID();
        }
    }
}
