//
// Created by student on 4/14/23.
//

#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"
#include <time.h>


using namespace std;

JobsList::JobEntry::JobEntry(int jobID, pid_t pid, const Command& command , bool stopped): jobID(jobID), pid(pid), command(command),
stopped(stopped)
        {
//    time_t* temptime = new time_t;
//    jobTime = time(temptime);
    time(jobTime);
}
JobsList::JobEntry::~JobEntry(){
}

void JobsList::JobEntry::printJob(){
    string cmd_s = _trim(string(command.cmd_line));
    string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));
    cout<< "[" << jobID << "] "<< firstword; << " "<< pid << " "<< jobTime;
    if(stopped)
    {
        cout<< "(stopped)" ;
    }
    cout << endl;

}
