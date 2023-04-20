//
// Created by student on 4/13/23.
//

#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"
#include <stdio.h>
#include <errno.h>


using namespace std;


/** change dir command */
ChangeDirCommand::ChangeDirCommand(const char *cmd_line, char **plastPwd, char*& previous)
        : BuiltInCommand(cmd_line), plastPwd(plastPwd), previous(previous) {
}

void ChangeDirCommand::execute() {
    if(plastPwd[2] != NULL){
        cerr << "smash error: too many arguments" << std::endl;
        return;
    }
    char buffer[COMMAND_ARGS_MAX_LENGTH];
    string first_arg = plastPwd[1];
    first_arg = _trim(first_arg);
    first_arg = first_arg.substr(0, first_arg.find_first_of(" \n"));
    if(first_arg == "-"){                                                   //special - flag
        if(previous == nullptr)
        {
            cerr << "smash error: cd: OLDPWD not set" <<std::endl;
            return;
        }
        else{
            char* temp = getcwd(NULL, COMMAND_ARGS_MAX_LENGTH);
            if(chdir(previous) == 0){
                delete previous;
                previous = temp;
            }
            else{
                perror("smash error: cd failed");
            }

        }
    }

    else {                                                                        //regular or ..
        char* temp = getcwd(NULL, COMMAND_ARGS_MAX_LENGTH);
        if (chdir(first_arg.c_str()) == 0) {
            delete previous;
            previous = temp;
        }
        else {
            perror("smash error: cd failed");
            delete temp;
        }
    }
}




/** change promt command */
ChPromtCommand::ChPromtCommand(const char *cmd_line, std::string& prompt) : BuiltInCommand(cmd_line), prompt(prompt) {

}

void ChPromtCommand::execute() {

    char* arguments[COMMAND_MAX_ARGS];
    _parseCommandLine(cmd_line, arguments);

    if (arguments[1] == NULL){
        prompt = "smash> ";
    }
    else{
        prompt = string(arguments[1]) + "> ";

    }
}

/** jobs  command */
JobsCommand::JobsCommand(const char *cmd_line): BuiltInCommand(cmd_line){
}

void JobsCommand::execute() {
SmallShell &smash = SmallShell::getInstance();
smash.getJoblist()->printJobsList();
}




/** fg command */

ForegroundCommand::ForegroundCommand(const char *cmd_line, JobsList *jobs) : BuiltInCommand(cmd_line), jobId(0), jobsList(jobs) {

}



void ForegroundCommand::execute() {
    char** arguments = makeArgsArr(cmd_line);
    int temp;

    //TODO: edge case when arguments start with "-"

    if (arguments[0] == NULL){
        if (jobsList->isEmpty()){
            cerr << "smash error: fg: jobs list is empty" << endl;
            return;
        }
        else{
            jobId = jobsList->getLargestJobID();
        }
        delete[] arguments;
    }
    else if(arguments[1] != NULL){
        cerr << "smash error: fg: invalid arguments" << std::endl;
        delete[] arguments;
        return;
    }
    else{
        //TODO: check minus case in piazza
        try {
            temp = stoi(string(arguments[0]));
        }
        catch (std::exception &) {   //bad argument
            cerr << "smash error: fg: invalid arguments" << std::endl;
            delete[] arguments;
            return;
        }
        jobId = temp;
    }

    auto& smashy = SmallShell::getInstance();
    auto jobEntry = jobsList->getJobById(jobId);

    if(jobEntry == nullptr){
        cerr << "smash error: fg: job-id " << jobId << " does not exist" << endl;
        return;
    }


    pid_t pid = jobEntry->getJobPID();

    smashy.updateForegroundCommandPID(pid);

    string meow = jobEntry->get_cmd_line();
    smashy.update_fg_cmd_line(meow);

    jobEntry->printJob_for_fg();
    kill(pid, SIGCONT);

    jobsList->removeJobById(jobId);

    waitpid(pid, nullptr, WUNTRACED);
}


BackgroundCommand::BackgroundCommand(const char *cmd_line, JobsList *jobs) : BuiltInCommand(cmd_line), jobList(jobs), jobId(0) {
    char** arguments = makeArgsArr(cmd_line);   //if only we had c++14...
    int temp;

    if (arguments[0] == NULL){
        if (jobList->isEmpty()){
            jobId = emptyListError;
            delete[] arguments;
            return;
        }
        jobId = jobList->getLargestJobID();
        delete[] arguments;
        return;
    }

    if (arguments[1] != NULL){
        jobId = badArgumentsError;
        delete[] arguments;
        return;
    }

    try {
        temp = stoi(string(arguments[0]));
    }
    catch(std::exception&) {   //bad argument
        jobId = badArgumentsError;
        delete[] arguments;
        return;
    }
    jobId = temp;
    delete[] arguments;
}

void BackgroundCommand::execute() {
    char** arguments = makeArgsArr(cmd_line);
    int temp;

    if (arguments[0] == NULL){
        jobId = jobList->getLargestStoppedJobID();
        if (jobId == 0){
            cerr << "smash error: bg: there is no stopped jobs to resume" << endl;
            delete[] arguments;
            return;
        }
    }
    else if(arguments[1] != NULL){
        cerr << "smash error: bg: invalid arguments" << endl;
        delete[] arguments;
    }

    else{
        //TODO: check minus case in piazza
        try {
            temp = stoi(string(arguments[0]));
        }
        catch (std::exception &) {   //bad argument
            cerr << "smash error: bg: invalid arguments" << std::endl;
            delete[] arguments;
            return;
        }
        jobId = temp;
    }

    auto& smashy = SmallShell::getInstance();
    auto jobEntry = jobList->getJobById(jobId);

    if(jobEntry == nullptr){
        cerr << "smash error: bg: job-id " << jobId << " does not exist" << endl;
        return;
    }

    pid_t pid = jobEntry->getJobPID();


    jobEntry->updateJobStoppedStatus();
    jobEntry->printJob_for_fg();
    kill(pid, SIGCONT);


}



KillCommand::KillCommand(const char *cmd_line, JobsList *jobs) : BuiltInCommand(cmd_line), jobsList(jobs) {

}

void KillCommand::execute() {
    char** arguments = makeArgsArr(cmd_line);

    int signum = removeMinusFromStringAndReturnAsInt(arguments[0]);
    if (signum == -1){
        std::cerr << "smash error: kill: invalid arguments" << endl;
        return;
    }

    int jobId;
    try{
        jobId = stoi(string(arguments[1]));
    }
    catch (std::exception& e){
        std::cerr << "smash error: kill: invalid arguments" << endl;
        return;
    }


    //checking signum
    if (signum > 30 || signum <= 0){
        cerr << "smash error: kill: invalid arguments" << endl;
        return;
    }

    //get pid
    auto jobEntry  = jobsList->getJobById(jobId);
    if (jobEntry == nullptr){
        cerr << "smash error: kill: job-id " << jobId << " does not exist" << endl;
        return;
    }
    pid_t pid = jobEntry->getJobPID();


    //updating jobList for specif signals that are sent

    if (signum == SIGCONT || signum == SIGSTOP){
        jobEntry->updateJobStoppedStatus();
    }


    //send messege
    kill(pid, signum);
    cout <<"signal number " << signum << " was sent to pid " << pid << endl;
}




