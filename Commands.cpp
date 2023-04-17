
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"

using namespace std;


#if 0
#define FUNC_ENTRY()  \
  cout << __PRETTY_FUNCTION__ << " --> " << endl;

#define FUNC_EXIT()  \
  cout << __PRETTY_FUNCTION__ << " <-- " << endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif


std::string _ltrim(const std::string& s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string _rtrim(const std::string& s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string _trim(const std::string& s)
{
    return _rtrim(_ltrim(s));
}

int _parseCommandLine(const char* cmd_line, char** args) {
  FUNC_ENTRY()
  int i = 0;
  std::istringstream iss(_trim(string(cmd_line)).c_str());
  for(std::string s; iss >> s; ) {
    args[i] = (char*)malloc(s.length()+1);
    memset(args[i], 0, s.length()+1);
    strcpy(args[i], s.c_str());
    args[++i] = NULL;
  }
  return i;

  FUNC_EXIT()
}

bool _isBackgroundComamnd(const char* cmd_line) {
  const string str(cmd_line);
  return str[str.find_last_not_of(WHITESPACE)] == '&';
}

bool isSpecialExternalCommand(const char* cmd_line){
    const string str(cmd_line);
    bool questionMark = str.find_first_of('?') != string::npos;
    bool starMark = str.find_first_of('*') != string::npos;
    return (questionMark || starMark);
}


void _removeBackgroundSign(char* cmd_line) {
  const string str(cmd_line);
  // find last character other than spaces
  unsigned int idx = str.find_last_not_of(WHITESPACE);
  // if all characters are spaces then return
  if (idx == string::npos) {
    return;
  }
  // if the command line does not end with & then return
  if (cmd_line[idx] != '&') {
    return;
  }
  // replace the & (background sign) with space and then remove all tailing spaces.
  cmd_line[idx] = ' ';
  // truncate the command line string up to the last non-space character
  cmd_line[str.find_last_not_of(WHITESPACE, idx) + 1] = 0;
}

char** makeArgsArr(const char *cmd_line){
    char *arguments[COMMAND_MAX_ARGS];
    int numberOfWords = _parseCommandLine(cmd_line, arguments);
    char** function_args = new char*[numberOfWords];
    for(int i=0; i< numberOfWords - 1; ++i){
        function_args[i] = arguments[i+1];
    }
    return function_args;
}


// TODO: Add your implementation for classes in Commands.h 

SmallShell::SmallShell():previousPath(nullptr) , shellPromt("smash> "), foregroundCommandPID(0), externalCommandInFgPointer(nullptr){
//    previousPath = nullptr;
//    shellPromt = "smash> ";
//    jobList =  JobsList();
}

SmallShell::~SmallShell() {
// TODO: add your implementation
}

void SmallShell::updatePreviousPath(char *path) {
    previousPath = path;
}

/**
* Creates and returns a pointer to Command class which matches the given command line (cmd_line)
*/
Command * SmallShell::CreateCommand(const char* cmd_line) {
  string cmd_s = _trim(string(cmd_line));
  string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));
  char *arguments[COMMAND_MAX_ARGS];
  int numberOfWords = _parseCommandLine(cmd_line, arguments);

//  if (firstWord.compare("pwd") == 0) {
//    return new GetCurrDirCommand(cmd_line);
//  }
  if (firstWord.compare("showpid") == 0) {
    return new ShowPidCommand(cmd_line);
  }
  else if(firstWord.compare("pwd") == 0){
      return new GetCurrDirCommand(cmd_line);
  }
  else if(firstWord.compare("cd") == 0){
      return new ChangeDirCommand(cmd_line, arguments, previousPath);
  }
  else if(firstWord.compare("chprompt") == 0){
      return new ChPromtCommand(cmd_line, shellPromt);
  }
  else if(firstWord.compare("jobs") == 0){
      return new JobsCommand(cmd_line);
  }
  else if(firstWord.compare("fg") == 0){
      return new ForegroundCommand(cmd_line, &jobList );
  }
  else if (firstWord.compare("kill") == 0){
      return new KillCommand(cmd_line, &jobList);
  }
  else {
      return new ExternalCommand(cmd_line);
  }
//  else if ...
//  .....
//  else {
//    return new ExternalCommand(cmd_line);
//  }

  return nullptr;
}

void SmallShell::executeCommand(const char *cmd_line) {
    auto& smashy = SmallShell::getInstance();
    smashy.reap();

   Command* cmd = CreateCommand(cmd_line);
    bool isBackground = _isBackgroundComamnd(cmd_line);

   //if command doesn't exist
   //TODO: I think they actualy wanted us to put sometihng real here. This is temporary.
   if (cmd == nullptr){
       printf("I don't know this command my good friend. Perhaps you are mistaken?\n");
       return;
   }
   cmd->execute();
   delete cmd;
  // Please note that you must fork smash process for some commands (e.g., external commands....)
}

char *SmallShell::getPreviousPath() {
    return previousPath;
}

std::string SmallShell::getPromt() const {
    return shellPromt;
}

void SmallShell::updateForegroundCommandPID(pid_t pid) {
    foregroundCommandPID = pid;
}

pid_t SmallShell::getForegroundCommandPID() const{
    return foregroundCommandPID;
}
JobsList* SmallShell::getJoblist() {
    return &jobList;
}

ExternalCommand *SmallShell::getExternalCommandInFgPointer() const {
    return externalCommandInFgPointer;
}

void SmallShell::setExternalCommandInFgPointer(ExternalCommand *ptr) {
    externalCommandInFgPointer = ptr;
}

void SmallShell::reap() {
    while (true){
        pid_t pid = waitpid(-1, nullptr, WNOHANG);
        if (pid == 0){
            cout << "no childs to kill" << endl;
            return;
        }
        auto jobToRemove = jobList.getJobByPID(pid);
        if(jobToRemove == nullptr){
            return;
        }
        cout << "I removed job " << jobToRemove->getJobID() << endl;
        jobList.removeJobById(jobToRemove->getJobID());
    }
}









//This is where our code begins!

//Command Thing
Command::Command(const char *cmd_line) : cmd_line(cmd_line) {

}

const char *Command::getCommand() const {
    return cmd_line;
}


//builtInCommnd
BuiltInCommand::BuiltInCommand(const char *cmd_line) : Command(cmd_line) {

}



/** showpid command    */

ShowPidCommand::ShowPidCommand(const char *cmd_line) : BuiltInCommand(cmd_line) {

}


void ShowPidCommand::execute() {
    printf("smash pid is %d\n",getpid());
}


/** pwd command      */


GetCurrDirCommand::GetCurrDirCommand(const char *cmd_line) : BuiltInCommand(cmd_line) {

}

void GetCurrDirCommand::execute() {
    char buffer[COMMAND_ARGS_MAX_LENGTH];
    std::cout << getcwd(buffer, COMMAND_ARGS_MAX_LENGTH);
    std::cout << "\n";
}

/**
 * function recieves a char* that starts with - and removes it. Allocates memory for the new string.
 * @param str
 * @return
 */
char* removeMinusFromStartOfString(char *str) {
    int len = strlen(str);
    if (len == 0 || str[0] != '-') {
        // Input string is empty or does not start with a dash
        return NULL;
    }
    // Copy the characters after the first dash to a new string
    char *result = new char[len];
    strncpy(result, str + 1, len - 1);
    result[len - 1] = '\0';
    return result;
}



int convertStringToInt(char* str){
    int temp;
    try{
        temp = stoi(string(str));
    }
    catch(std::exception& e){
        cout << "something bad has happened my friend" << endl; //TODO: something real
        return -1;
    }
    return temp;
}


int removeMinusFromStringAndReturnAsInt(char* str){
    char* temp = removeMinusFromStartOfString(str);
    if (temp == NULL){
        return -1;
    }
    else{
        int num = convertStringToInt(temp);
        return num;
    }
}





