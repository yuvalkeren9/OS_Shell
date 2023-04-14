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
    pid_t pid = fork();
    if (pid == 0){ //child
        setpgrp();
        if (execvp(arguments[0],arguments)==-1)
        {
            perror("smash error: execv failed");
        }
    }
    else if( pid == -1){   //error
        //TODO: throw error
        perror("smash error: fork failed");
    }
    else{
        waitpid(pid, nullptr, 0);
    }
}




