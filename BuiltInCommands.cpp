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
        std::cout << "My good friend, too many arguments. fuck off. \n";
        return;
    }
    char buffer[COMMAND_ARGS_MAX_LENGTH];
    string first_arg = plastPwd[1];
    first_arg = _trim(first_arg);
    first_arg = first_arg.substr(0, first_arg.find_first_of(" \n"));
    if(first_arg == "-"){
        if(previous == nullptr)
        {
            std::cout << "smash error: cd: OLDPWD not set\n";
            //TODO: error handling something
            return;
        }
        else{
            chdir(previous);
            char* temp = getcwd(NULL, COMMAND_ARGS_MAX_LENGTH);
            delete previous;
            previous = temp;
        }
    }

    else {
        if (chdir(first_arg.c_str()) == 0) {
            char* temp = getcwd(NULL, COMMAND_ARGS_MAX_LENGTH);
            delete previous;
            previous = temp;
        }
        else {
            std::cout <<"you mistaken my friend. maybe not real directory? maybe no privlage? \n";
            //TODO: some kind error (perror thing)
        }
    }


}

