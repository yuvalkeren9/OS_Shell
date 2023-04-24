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


using namespace std;

JobsList::JobEntry::JobEntry(int jobID, pid_t pid, string cmd_line , bool stopped): jobID(jobID), pid(pid), cmd_line(cmd_line),
stopped(stopped), wasJobInForeground(false)
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
void JobsList::JobEntry::printJob_for_fg() const{
    cout<< cmd_line  << " : "<< pid << "\n";
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
    if(stopped)
    {
    stopped= false;
    } else{
        stopped=true;
    }
}

bool JobsList::JobEntry::isStopped() const {
    return stopped;
}

bool JobsList::JobEntry::getwasJobInForeground() const {
    return wasJobInForeground;
}

void JobsList::JobEntry::setwasJobInForeground() {
    wasJobInForeground = true;
}


JobsList::JobsList():numOfJobs(0), jobsVector(), jobInForeground(nullptr), jobInForegroundIndex(-1){   //made little change here
}

void JobsList::addJob(const char *cmd_line,pid_t pid, bool isStopped) {

    //if there is a command that was in the back, and then brought to the front, put it back!
    //return

    if (jobInForeground != nullptr){
        jobsVector.insert(jobsVector.begin()+jobInForegroundIndex,jobInForeground);
        jobInForeground->setJobStoppedStatus(true);
        jobInForeground = nullptr;
        jobInForegroundIndex = -1;
        return;
    }



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
    for(const JobEntry *job:jobsVector){
      pid_t pid_toKill= job->getJobPID();
      kill(pid_toKill,SIGKILL);
      sleep(1);
      job->printJob_for_fg();
    }
}

int JobsList::getNumOfJobs() const {
    this->jobsVector.size();
}

void JobsList::removeJobByIdFG(int jobId) {

    int i=0;
    for( JobEntry *job:jobsVector) {
        if(job->getJobID()==jobId){   //there used to be a const here
            jobInForeground = job;
            jobInForegroundIndex = i;
            this->jobsVector.erase(jobsVector.begin()+i);
            break;
        }
        i++;
    }
    jobInForeground->setwasJobInForeground();
    cout <<"my duck is cool" << endl;

}

JobsList::JobEntry *JobsList::getJobInForeground() const {
    return jobInForeground;
}

void JobsList::removeJobInForeground() {
    cout << "my name is maor edri" << endl;
    int i=0;
    int jobId = jobInForeground->getJobID();
    for( JobEntry *job:jobsVector) {
        if(job->getJobID()==jobId){   //there used to be a const here
            jobInForeground = nullptr;
            jobInForegroundIndex = -1;
            this->jobsVector.erase(jobsVector.begin()+i);
            break;
        }
        i++;
    }
}

void JobsList::JobEntry::setJobStoppedStatus(bool cond){
    stopped = cond;
}

