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
JobsCommand::JobsCommand(const char *cmd_line, JobsList *jobs): BuiltInCommand(cmd_line),jobs(jobs){
}

void JobsCommand::execute() {
jobs->printJobsList();
}



