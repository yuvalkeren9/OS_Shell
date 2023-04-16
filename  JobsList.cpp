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
#include <stl.h>


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
    cout<< "[" << jobID << "] "<< cmd_line  << " "<< pid << " ";
  //  cout << std::ctime(jobTime);
    time_t now = time(nullptr);
    double difference = difftime(now, *jobTime);
   cout << difference <<" sec";
    if(stopped)
    {
        cout<< "(stopped)" ;
    }
    cout << endl;

}
 int JobsList::JobEntry::getJobID() const {
    return jobID;
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

int JobsList::getLargestJobID() {


    int max =0;
    for(const JobEntry *job:jobsVector){
        if(job->getJobID()> max)
        {
            max=job->getJobID();
        }
    }
    return  max;
}

void JobsList::printJobsList() {
    for(const JobEntry *job:jobsVector) {
        job->printJob();
    }
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
