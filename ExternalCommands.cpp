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
    bool isBackground = _isBackgroundComamnd(cmd_line);
    bool isSpecialCommand = isSpecialExternalCommand(cmd_line);

    char cmd_line_edit[COMMAND_ARGS_MAX_LENGTH];
    strcpy(cmd_line_edit, cmd_line);

    if (isBackground){
        _removeBackgroundSign(cmd_line_edit);

    }

    int numberOfWords = _parseCommandLine(cmd_line_edit, arguments);
//    char **function_args = new char *[numberOfWords];
//    for (int i = 0; i < numberOfWords - 1; ++i) {
//        function_args[i] = arguments[i + 1];
//    }
//    if (numberOfWords == 0) {
//        function_args = NULL;
//    }

    pid_t pid = fork();
    if (pid == 0) {                                                         //child
        setpgrp();
        cout << "!!!!";
        if (!isSpecialCommand){
            if (execvp(arguments[0],arguments)==-1)//changed here execvp to execv because execvp cant run a.out (as requested) only ./a.out
            {
                cout << "first arg = "<< arguments[0] << endl;
                cout << "second arg = "<< arguments[1] << endl;
                cout << "third arg = "<< arguments[2] << endl;
                if(arguments[3]== nullptr)
                    cout << "forth arg = null"<< arguments[2] << endl;
 //               cout << "forth arg = "<< arguments[4] << endl;
//                cout << "fifth arg = "<< arguments[5] << endl;
               // fprintf(stderr,"execvp() failed");
                perror("smash error: execv failed");
            }
        }
        else{
            char bashString[10] = {'/','b','i','n','/','b','a','s','h','\0'};  //this is sick. LOL
            char bashFlagString[3] = {'-','c','\0'};
            char* bashCommandString[4] = {bashString, bashFlagString, cmd_line_edit, NULL};

            if(execv("/bin/bash",bashCommandString) == -1){     //run bash
                perror("smash error: execv failed");
            };
        }
    }



    else if( pid == -1){                                            //error
        //TODO: throw error
        perror("smash error: fork failed");
    }



    else {                                                          //parnet
        if (isBackground){
            //don't wait
            cout << "I am in the background!\n";
        }
        else {
            waitpid(pid, nullptr, 0);
        }
    }
}





