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

std::string _trimBuiltInCommand(const std::string& s);



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
    char buffer[1000];
    string first_arg = plastPwd[1];
    first_arg = _trimBuiltInCommand(first_arg);
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
            char* temp = getcwd(NULL, 1000);
            delete previous;
            previous = temp;
        }
    }

    else {
        if (chdir(first_arg.c_str()) == 0) {
            char* temp = getcwd(NULL, 1000);
            delete previous;
            previous = temp;
        }
        else {
            std::cout <<"you mistaken my friend. maybe not real directory? maybe no privlage? \n";
            //TODO: some kind error (perror thing)
        }
    }


}


//garbage programming


std::string _ltrimBuiltInCommand(const std::string& s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string _rtrimBuiltInCommand(const std::string& s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string _trimBuiltInCommand(const std::string& s)
{
    return _rtrimBuiltInCommand(_ltrimBuiltInCommand(s));
}
