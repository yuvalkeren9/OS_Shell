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


using std::string;
using std::cout;
using namespace std;



/** change dir command */
ChangeDirCommand::ChangeDirCommand(const char *cmd_line, char **plastPwd, char*& previous)
        : BuiltInCommand(cmd_line), plastPwd(plastPwd), previous(previous) {
}

void ChangeDirCommand::execute() {
//    string first_arg = _trim(string(cmd_line));
//    string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));
    if(plastPwd[2] != NULL){
      //  std::cout << "My good friend, too many arguments. fuck off. \n";
        cout << "smash error: too many arguments" << std::endl;
        return;
    }
    char buffer[COMMAND_ARGS_MAX_LENGTH];
    string first_arg = plastPwd[1];
    first_arg = _trim(first_arg);
    first_arg = first_arg.substr(0, first_arg.find_first_of(" \n"));
    if(first_arg == "-"){                                                   //special - flag
        if(previous == nullptr)
        {
          //  std::cout << "smash error: cd: OLDPWD not set\n";
            cout << "smash error: cd: OLDPWD not set" <<std::endl;
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
    char** arguments = makeArgsArr(cmd_line);   //if only we had c++14...
    int temp;

    if (arguments[0] == NULL){
        if (jobsList->isEmpty()){
            jobId = emptyListError;
            delete[] arguments;
            return;
        }
        jobId = jobsList->getLargestJobID();
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



void ForegroundCommand::execute() {
    char** arguments = makeArgsArr(cmd_line);

    //TODO: edge case when arguments start with "-"

    if (arguments[0] == NULL){
        if (jobsList->isEmpty()){
            cout << "smash error: fg: jobs list is empty" << endl;
        }
        else{
            jobId = jobsList->getLargestJobID();
        }
        delete[] arguments;
        return;
    }
    else if(arguments[1] != NULL){
        cout << "smash error: fg: invalid arguments" << std::endl;
        delete[] arguments;
        return;
    }


    int temp;
    try {
        temp = stoi(string(arguments[0]));
    }
    catch (std::exception &) {   //bad argument
        cout << "smash error: fg: invalid arguments" << std::endl;
        delete[] arguments;
        return;
    }

    jobId = temp;


    auto& smashy = SmallShell::getInstance();
    auto jobEntry = jobsList->getJobById(jobId);

    if(jobEntry == nullptr){
        cout << "smash error: fg: job-id " << jobId << " does not exist" << endl;
        return;
    }


    pid_t pid = jobEntry->getJobPID();

    smashy.updateForegroundCommandPID(pid);
    smashy.update_fg_cmd_line(jobEntry->get_cmd_line());
    //TODO: print the job cmd_line thingy and the pid

    kill(pid, SIGCONT);

    jobsList->removeJobById(jobId);

    waitpid(pid, nullptr, WUNTRACED);
}


BackgroundCommand::BackgroundCommand(const char *cmd_line, JobsList *jobs) : BuiltInCommand(cmd_line) {
    char** arguments = makeArgsArr(cmd_line);


}

void BackgroundCommand::execute() {

}



KillCommand::KillCommand(const char *cmd_line, JobsList *jobs) : BuiltInCommand(cmd_line), jobsList(jobs) {

}

void KillCommand::execute() {
    //TOdo: check for arguments (inclduing if a too large signal was sent
    char** arguments = makeArgsArr(cmd_line);

    int signum = removeMinusFromStringAndReturnAsInt(arguments[0]);  //TODO: check for errors
    int jobId = stoi(string(arguments[1]));  //TODO: do try catch

    //get pid
    pid_t pid = jobsList->getJobById(jobId)->getJobPID();

    //send messege
    kill(pid, signum);
    cout <<"signal number " << signum << " was sent to pid " << pid << endl;
}




