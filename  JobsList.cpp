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

JobsList::JobEntry::JobEntry(const Command& command , bool stopped, int jobID):command(command),
stopped(stopped), jobID(jobID)
        {
    time_t* temp_time =new time_t;
    time =time::time(temp_time);
}
