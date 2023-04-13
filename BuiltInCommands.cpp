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

/** change dir command */
ChangeDirCommand::ChangeDirCommand(const char *cmd_line, char **plastPwd)
        : BuiltInCommand(cmd_line), plastPwd(plastPwd) {

}

void ChangeDirCommand::execute() {

//    string first_arg = _trim(string(cmd_line));
//    string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));

    if(plastPwd[2] != NULL){
        std::cout << "My good friend, too many arguments. fuck off. \n";
        return;
    }
    string first_arg = plastPwd[1];
    std::cout << "The first argument is:" << first_arg << "issss" << "\n";

    chdir(plastPwd[1]);

}
