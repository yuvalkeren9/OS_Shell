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


using namespace std;

ExternalCommand::ExternalCommand(const char *cmd_line) : Command(cmd_line) {

}

void ExternalCommand::execute() {

    char *arguments[COMMAND_MAX_ARGS];
    int numberOfWords = _parseCommandLine(cmd_line, arguments);
    char** function_args = new char*[numberOfWords];
    for(int i=0; i< numberOfWords - 1; ++i){
        function_args[i] = arguments[i+1];
    }


    pid_t pid = fork();
    if (pid == 0){ //child
        setpgrp();
        execv(arguments[0],function_args);
    }
    else if( pid == -1){   //error
        //TODO: throw error
    }
    else{
        waitpid(pid, nullptr, 0);
    }
}




