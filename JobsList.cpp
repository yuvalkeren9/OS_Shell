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
#include <ctime>
#include <cassert>


using namespace std;

JobsList::JobEntry::JobEntry(int jobID, pid_t pid, string cmd_line , bool stopped): jobID(jobID), pid(pid), cmd_line(cmd_line),
stopped(stopped)
        {
    time_t* temptime = new ::time_t ;
    time(temptime);
    jobTime=temptime;
}
JobsList::JobEntry::~JobEntry(){
    delete jobTime;
}

void JobsList::JobEntry::printJob() const{
    cout<< "[" << jobID << "] "<< cmd_line  << " : "<< pid << " ";
  //  cout << std::ctime(jobTime);
    time_t now = time(nullptr);
    double difference = difftime(now, *jobTime);
   cout << difference <<" secs";
    if(stopped)
    {
        cout<<" (stopped)" ;
    }
    cout << endl;

}
void JobsList::JobEntry::printJob_for_fg() const{
    cout<< cmd_line  << " : "<< pid << endl;
}

void JobsList::JobEntry::printJobForKillAll() const{
    cout << pid << ": " << cmd_line << endl;
}
 int JobsList::JobEntry::getJobID() const {
    return jobID;
}

pid_t JobsList::JobEntry::getJobPID() const {
    return pid;
}


std::string JobsList::JobEntry::get_cmd_line() const{
    return cmd_line;
}

void JobsList::JobEntry::updateJobStoppedStatus() {
    if(stopped== true)
    {
    stopped= false;
    } else{
        stopped=true;
    }
}

bool JobsList::JobEntry::isStopped() const {
    return stopped;
}

void JobsList::JobEntry::resetTime() {
    delete jobTime;
    time_t* temptime = new ::time_t ;
    time(temptime);
    jobTime=temptime;
    stopped = true; // the only case that the time need to be reset is the case that the process stopped
}


JobsList::JobsList():numOfJobs(0){
}

void JobsList::addJob(const char *cmd_line,pid_t pid, bool isStopped) {
    int newJobID = getLargestJobID()+1;
    char *temp= new char[strlen(cmd_line)];
    strcpy(temp,cmd_line);
    string temp1=string (temp);
    JobEntry* newJob = new JobEntry(newJobID,pid,temp,isStopped);
    delete temp;
    numOfJobs++;
    jobsVector.push_back(newJob);
}

int JobsList::getLargestJobID() const {


    int max =0;
    for(const JobEntry *job:jobsVector){
        if(job->getJobID()> max)
        {
            max = job->getJobID();
        }
    }
    return  max;
}

void JobsList::printJobsList() {
    for(const JobEntry *job:jobsVector) {
        job->printJob();
    }
}

JobsList::JobEntry *JobsList::getJobById(int jobId) {
    for(JobEntry* jobEntry : jobsVector){
        if (jobEntry->getJobID() == jobId){
            return jobEntry;
        }
    }
    return nullptr;   //if was not found
}


JobsList::JobEntry *JobsList::getJobByPID(pid_t jobPID) const {
    for(JobEntry* jobEntry : jobsVector){
        if (jobPID == jobEntry->getJobPID()){
            return jobEntry;
        }
    }
    return nullptr;   //if was not found
}

bool JobsList::isEmpty() const {
    return jobsVector.empty();
}



void JobsList::removeJobById(int jobId) {
    int i=0;
    for(const JobEntry *job:jobsVector) {
        if(job->getJobID()==jobId){
            this->jobsVector.erase(jobsVector.begin()+i);
            break;
        }
        i++;
    }
}

int JobsList::removeJobByPID(int jobPID) {
    int i=0;
    for(const JobEntry *job:jobsVector) {
        if(job->getJobPID()==jobPID){
            this->jobsVector.erase(jobsVector.begin()+i);
            return 0;
        }
        i++;
    }
    return -1;
}

int JobsList::getLargestStoppedJobID() const {
        int max =0;
        for(const JobEntry *job:jobsVector){
            if(job->isStopped())
            {
                if(job->getJobID()> max)
                {
                    max = job->getJobID();
                }
            }
        }
        return  max;
}

void JobsList::killAllJobs() {
    for(const JobEntry *job : jobsVector){
      pid_t pid_toKill= job->getJobPID();
      kill(pid_toKill,SIGKILL);
      sleep(1);
      job->printJobForKillAll();
    }
}

int JobsList::getNumOfJobs() const {
    return this->jobsVector.size(); //elad yaloser
}

//yolo2
void JobsList::resetJobTime(pid_t pid) {
    assert(pid >=0 );
    for(JobEntry *job : jobsVector){
        if( job->getJobPID() == pid){
            job->resetTime();
            return;
        }
    }
    return;
}
