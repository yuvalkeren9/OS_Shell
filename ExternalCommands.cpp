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
#include "signals.h"

using namespace std;

ExternalCommand::ExternalCommand(const char *cmd_line) : Command(cmd_line) {

}

void ExternalCommand::execute() {

    auto& smashy = SmallShell::getInstance();
    char *arguments[COMMAND_MAX_ARGS];
    bool isBackground = _isBackgroundComamnd(cmd_line);
    bool isSpecialCommand = isSpecialExternalCommand(cmd_line);
    bool isTimeout = isTimeoutCommand(cmd_line);
    char cmd_line_edit[COMMAND_ARGS_MAX_LENGTH];
    strcpy(cmd_line_edit, cmd_line);

    if (isBackground){
        _removeBackgroundSign(cmd_line_edit);
    }

    int numberOfWords = _parseCommandLine(cmd_line_edit, arguments);



    pid_t pid = fork();
    if (pid == 0) {                                                         //child
        setpgrp();
        if (!isSpecialCommand){
            if (execv(arguments[0], arguments) == -1){
                if (execvp(arguments[0],arguments)==-1)
                {
                    perror("smash error: execv failed");
                    exit(-1);
                }
            }
        }
        else{
            char bashString[10] = {'/','b','i','n','/','b','a','s','h','\0'};  //this is sick. LOL
            char bashFlagString[3] = {'-','c','\0'};
            char* bashCommandString[4] = {bashString, bashFlagString, cmd_line_edit, NULL};

            if(execv("/bin/bash",bashCommandString) == -1){     //run bash
                perror("smash error: execv failed");
                exit(-1);
            };
        }
    }



    else if( pid == -1){                                            //error
        perror("smash error: fork failed");
    }



    else {                                                          //parnet
        if (isBackground){
            smashy.getJoblist()->addJob(cmd_line,pid,false);
        }
        else {
//            smashy.setExternalCommandInFgPointer(this);
            smashy.update_fg_cmd_line(cmd_line);
            smashy.updateForegroundCommandPID(pid);
            waitpid(pid, nullptr, WUNTRACED);
            smashy.updateForegroundCommandPID(0);
        }
    }
}





