
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/*
* Macro providing a “safe” way to invoke system calls
*/
#define DO_SYS_CUSTOM( syscall, sysCallName ) do { \
/* safely invoke a system call */ \
if( (syscall) == -1 ) {                            \
    perror("smash error: " + sysCallName + " failed" ); \
    return; \
    } \
} while( 0 )                                       \

#define PIPE_READ 0
#define PIPE_WRITE 1

#define STDIN_FDT 0
#define STDOUT_FDT 1
#define STDERROR_FDT 2
#define SYSCALL_FAILED -1

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





/** -----------------------------------------------------------------------------------------------------------------------------------------------------------------  **/
/** ----------------------------------------------------------   helping function (string edit , etc..)    ----------------------------------------------------------  **/
/** -----------------------------------------------------------------------------------------------------------------------------------------------------------------  **/


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

string cutUntillChar(const char* cmd_line , char character){// return value not includibg the char sent
    string str;
    for (int i = 0; i < strlen(cmd_line); ++i){
        if (cmd_line[i] == character){
            break;
        }
        else{
            str += cmd_line[i];
        }
    }
    return str;
}

string cutAfterChar(const char* cmd_line , char character){// return value not includibg the char sent
    string str;
    int firstAppearIndex = (string(cmd_line).find_first_of(character));
    if(firstAppearIndex!=string(cmd_line).length())
    {
        firstAppearIndex++;
    }
    for (int i = 0; i < strlen(cmd_line)-firstAppearIndex; ++i) {
        str += cmd_line[i + firstAppearIndex];
    }
    return str;
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





/**
 * This function recives the arguments char** from parseCmdLine and returns in which token a ceratin char is (like > or |)
 * @param str string to find
 * @param arguments cmdline, parsed
 * @param numOfArgs many tokens are in there
 * @return the index of the string. If it is not found, then -1 is returned
 *
 */
int findFirstCharInArgs(const string& str, char** arguments, int numOfArgs){
    for (int i=0; i < numOfArgs; ++i){
        if (string(arguments[i]) == str ){
            return i;
        }
    }
    //if failed
    return -1;
}


/**
 * return the first location of > or >>. If no crocidle, then returns -1
 * @param arguments
 * @param numberOfWords
 * @return
 */
int getCrocLocation(char** arguments, int numberOfWords){
    int index = findFirstCharInArgs(">", arguments, numberOfWords);
    if (index != -1){
        return index;
    }
    else{
        index = findFirstCharInArgs(">>", arguments, numberOfWords);
        if (index != -1){
            return index;
        }
        else{
            return -1;
        }
    }
}

int getPipeLocation(char** arguments, int numberOfWords){
    int index = findFirstCharInArgs("|", arguments, numberOfWords);
    if (index != -1){
        return index;
    }
    else{
        index = findFirstCharInArgs("|&", arguments, numberOfWords);
        if (index != -1){
            return index;
        }
        else{
            return -1;
        }
    }
}



/** -----------------------------------------------------------------------------------------------------------------------------------------------------------------  **/
/** ----------------------------------------------------------   smallShell implementation    ----------------------------------------------------------  **/
/** -----------------------------------------------------------------------------------------------------------------------------------------------------------------  **/



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


  string cmd_line_as_string = cmd_line;

  bool isRedirectionCommand = (cmd_line_as_string.find_first_of('>') != string::npos);
  bool isPipeCommand(cmd_line_as_string.find_first_of('|') != string::npos);
  BuiltInCommand* command = checkCmdForBuiltInCommand(cmd_line);

  if (isRedirectionCommand){
      return new RedirectionCommand(cmd_line);
  }
  else if(isPipeCommand){
      return new PipeCommand(cmd_line);
  }
  else if( command != nullptr){
      return command;
  }
  else {
      return new ExternalCommand(cmd_line);
  }
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
        else if ( pid == -1){
            cout << "I have no kids.. I am so sad ):" << endl;
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



BuiltInCommand* SmallShell::checkCmdForBuiltInCommand(const char* cmd_line){

    assert(cmd_line != nullptr);
    //init annoying stuff for backward compability
    char** arguments = new char*[COMMAND_MAX_ARGS];
    int numberOfWords = _parseCommandLine(cmd_line, arguments);


    //deal with & case
    char cmd_line_edit[COMMAND_ARGS_MAX_LENGTH];
    strcpy(cmd_line_edit, cmd_line);
    if(_isBackgroundComamnd(cmd_line)){
        _removeBackgroundSign(cmd_line_edit);
    }

    string cmd_s = _trim(string(cmd_line_edit));
    string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));

    if (firstWord == "pid") {
        return new ShowPidCommand(cmd_line_edit);
    }
    else if(firstWord == "showpid"){
        return new GetCurrDirCommand(cmd_line);
    }
    else if(firstWord == "cd"){
        return new ChangeDirCommand(cmd_line, arguments, previousPath);
    }
    else if(firstWord == "chprompt"){
        return new ChPromtCommand(cmd_line, shellPromt);
    }
    else if(firstWord == "jobs"){
        return new JobsCommand(cmd_line);
    }
    else if(firstWord == "fg"){
        return new ForegroundCommand(cmd_line, &jobList );
    }
    else if(firstWord == "kill"){
        return new KillCommand(cmd_line, &jobList);
    }
    else if(firstWord == "bg"){
        return new BackgroundCommand(cmd_line, &jobList);
    }
    else if(firstWord == "quit"){
        return new QuitCommand(cmd_line, &jobList);
    }
    else if(firstWord == "getfiletype"){
        return new GetFileTypeCommand(cmd_line);
    }
    else{
        return nullptr;
    }
}











/** -----------------------------------------------------------------------------------------------------------------------------------------------------------------  **/
/** ----------------------------------------------------------   commands Implementation    -------------------------------------------------------------------------  **/
/** -----------------------------------------------------------------------------------------------------------------------------------------------------------------  **/



Command::Command(const char *cmd_line) : cmd_line(cmd_line) {

}

const char *Command::getCommand() const {
    return cmd_line;
}


//builtInCommnd
BuiltInCommand::BuiltInCommand(const char *cmd_line) : Command(cmd_line) {

}



/**-------------------------------------------------- showpid command ----------------------------------------------------------   */

ShowPidCommand::ShowPidCommand(const char *cmd_line) : BuiltInCommand(cmd_line) {

}


void ShowPidCommand::execute() {
    printf("smash pid is %d\n",getpid());
}



/**-------------------------------------------------- pwd command  ----------------------------------------------------------   */

GetCurrDirCommand::GetCurrDirCommand(const char *cmd_line) : BuiltInCommand(cmd_line) {

}

void GetCurrDirCommand::execute() {
    char buffer[COMMAND_ARGS_MAX_LENGTH];
    std::cout << getcwd(buffer, COMMAND_ARGS_MAX_LENGTH);
    std::cout << "\n";
}




/**-------------------------------------------------- Quit command  ----------------------------------------------------------   */

QuitCommand::QuitCommand(const char *cmd_line, JobsList *jobs): BuiltInCommand(cmd_line),jobs(jobs) {
}

void QuitCommand::execute() {
    SmallShell& smashy = SmallShell::getInstance();
    char** arguments= makeArgsArr(cmd_line);
    string str = arguments[0];
    if(str.compare("kill")==0){
        cout << "sending SIGKILL signal to "<< smashy.getJoblist()->getNumOfJobs()<<" jobs:"<< endl;
        jobs->killAllJobs();
    }
    sleep(2);
    smashy.reap();
    exit(0);
}



/**-------------------------------------------------- Redirection command  ----------------------------------------------------------   */

RedirectionCommand::RedirectionCommand(const char *cmd_line) : Command(cmd_line) {
}

void RedirectionCommand::execute() {

    //TODO: ignore &

    auto& smashy = SmallShell::getInstance();
    string cmd_s = _trim(string(cmd_line));
    string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));
    char *arguments[COMMAND_MAX_ARGS];
    int numberOfWords = _parseCommandLine(cmd_line, arguments);

    int crocLocationIndex = getCrocLocation(arguments, numberOfWords);

    string meow1 = cutUntillChar(cmd_line,'>');
    const char * cuttedCommand = meow1.c_str();
    auto commandToExecutre = smashy.CreateCommand(cuttedCommand);


    enum RedirectionType {OneCrocodile, TwoCrocodile, Other};
    RedirectionType redirectionType;
    if (string(arguments[crocLocationIndex]) == ">"){
        cout << "OneCroc" << endl;
        redirectionType = OneCrocodile;
    }
    else if(string(arguments[crocLocationIndex]) == ">>"){
        cout << "TwoCroc" << endl;

        redirectionType = TwoCrocodile;
    }
    else{
        redirectionType = Other;
    }

    if (redirectionType == RedirectionType::Other){
        cout << "Invalid arguments thing" << endl;
        //TODO: real error message
        return;
    }

    //so we can put std::cout back in FDT after we are done with redirection
    int stdout_fd = dup(1);
    if(stdout_fd == SYSCALL_FAILED){
        perror("first dup  error");
    }

   if(close(1)== SYSCALL_FAILED){
       perror("first close  error");
   }

    int fileToOpenIndex = crocLocationIndex + 1;
    switch (redirectionType){
        case OneCrocodile:
            open(arguments[fileToOpenIndex],O_CREAT | O_RDWR, S_IRWXU);
            perror("one corocidle error");
            break;
        case TwoCrocodile:
            open(arguments[fileToOpenIndex],O_CREAT | O_RDWR | O_APPEND, S_IRWXU); //TODO: figure ou what positions to give
            perror("two corocidle error");

            break;
        default:
            cout << "if im here, something has gone terribly wrong.";
            break;
    }


    commandToExecutre->execute();

    if(close(1)==SYSCALL_FAILED){
        perror("second close  error");
    }

    if(dup2(stdout_fd,1) == SYSCALL_FAILED){
        perror("second dup  error");
    }

    if(close(stdout_fd)==SYSCALL_FAILED){
        perror("third close error");
    }

}




/**-------------------------------------------------- Pipe command  ----------------------------------------------------------   */

PipeCommand::PipeCommand(const char *cmd_line) : Command(cmd_line)  {

}

void PipeCommand::execute() {

    //TODO: ignore &
    //TODO: instead of always returning, maybe do a "restore" function" for edge cases

    auto& smashy = SmallShell::getInstance();
    string cmd_s = _trim(string(cmd_line));
    string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));
    char *arguments[COMMAND_MAX_ARGS];
    int numberOfWords = _parseCommandLine(cmd_line, arguments);

    int pipeLocationIndex = getPipeLocation(arguments, numberOfWords);


    enum RedirectionType {CoutPipe, CerrPipe, Other};
    RedirectionType redirectionType;
    if (string(arguments[pipeLocationIndex]) == "|"){
        redirectionType = CoutPipe;
    }
    else if(string(arguments[pipeLocationIndex]) == "|&"){
        redirectionType = CerrPipe;
    }
    else{
        redirectionType = Other;
    }

    if (redirectionType == RedirectionType::Other){
        cout << "Invalid arguments thing" << endl;
        assert(false);
        return;
    }


    //creating the command objects

    string temp = cutUntillChar(cmd_line,'|');
    const char * cuttedCommand = temp.c_str();
    auto firstCommand = smashy.CreateCommand(cuttedCommand);

    char letter;
    if (redirectionType == RedirectionType::CoutPipe){
        letter = '|';
    }
    else{
        letter = '&';
    }
    const char* cmd_lineasfasf = cmd_line;
    string temp2 = cutAfterChar(cmd_lineasfasf, letter);
    const char * cuttedCommand2 = temp2.c_str();

    auto secondCommand = smashy.CreateCommand(cuttedCommand2);





    //so we can put std::cout back in FDT after we are done with redirection
    int stdin_fd = dup(STDIN_FDT);
    if (stdin_fd == -1){
        perror("smash error: dup failed");
        return;
    }

    int stdout_fd = dup(STDOUT_FDT);
    if (stdout_fd == -1){
        perror("smash error: dup failed");
        return;
    }

    int stderror_fd = dup(STDERROR_FDT);
    if (stderror_fd == -1){
        perror("smash error: dup failed");
        return;
    }


    int my_pipe[2];
    if(pipe(my_pipe) == -1 ){
        perror("smash error: pipe failed");
        return;
    }


    switch (redirectionType){
        case CoutPipe:
            if(dup2(my_pipe[PIPE_WRITE], STDOUT_FDT) == -1){
                perror("smash error: dup2 failed");
                return;
            }
            if(close(my_pipe[PIPE_WRITE]) == -1){
                perror("smash error: close failed");
                return;
            }
            break;
        case CerrPipe:
            if( dup2(my_pipe[PIPE_WRITE], STDERROR_FDT) == -1){
                perror("smash error: dup2 failed");
                return;
            }
            if(close(my_pipe[PIPE_WRITE]) == -1){
                perror("smash error: close failed");
                return;
            }
            break;
        default:
            cout <<"VERY BVERY BAD" << endl;
    }

    firstCommand->execute();
    delete firstCommand;

    switch (redirectionType){
        case CoutPipe:
            if (dup2(stdout_fd, STDOUT_FDT) == -1 ){
                perror("smash error: dup2 failed");
                return;
            }

            if( close(stdout_fd) == -1){
                perror("smash error: close failed");
                return;
            };

            break;
        case CerrPipe:
            if( dup2(stderror_fd, STDERROR_FDT) == 1){
                perror("smash error: dup2 failed");
                return;
            }
            if (close(stderror_fd) == -1){
                perror("smash error: close failed");
                return;
            }
            break;
        default:
            cout <<"VERY BVERY BAD meow" << endl;
    }



    if (close(STDIN_FDT) == -1){
        perror("smash error: close failed");
        return;
    }
    if(dup2(my_pipe[PIPE_READ], STDIN_FDT) == -1){
        perror("smash error: dup2 failed");
        return;
    }


    secondCommand->execute();
    delete secondCommand;



    if(dup2(stdin_fd, STDIN_FDT) == -1){
        perror("smash error: dup2 failed");
        return;
    }


    if(close(my_pipe[0]) == -1){
        perror("smash error12345: close failed");
        return;
    }
}



/**-------------------------------------------------- Get File Type command  ----------------------------------------------------------   */

GetFileTypeCommand::GetFileTypeCommand(const char *cmd_line) : BuiltInCommand(cmd_line) {
}

void GetFileTypeCommand::execute() {
    auto& smashy = SmallShell::getInstance();
    string cmd_s = _trim(string(cmd_line));
    char *arguments[COMMAND_MAX_ARGS];
    int numberOfWords = _parseCommandLine(cmd_line, arguments);
    if(arguments[2] != NULL){
        cerr << "smash error: too many arguments" << std::endl;
        return;
    }
    const char *path = arguments[1];
    string pathStr =string (path);
    struct stat fileStats;
    if(stat(path,&fileStats)==SYSCALL_FAILED){
        perror("stat function failed");
        return;
    }
    const auto  fileSize =fileStats.st_size;
    const auto fileType = fileStats.st_mode;
    string strFileType;
    switch (fileStats.st_mode & S_IFMT) {
        case S_IFBLK:  strFileType = "block device";                 break;
        case S_IFCHR:  strFileType = "character device";             break;
        case S_IFDIR:  strFileType = "directory";                    break;
        case S_IFIFO:  strFileType = "FIFO/pipe";                    break;
        case S_IFLNK:  strFileType = "symlink";                      break;
        case S_IFREG:  strFileType = "regular file";                 break;
        case S_IFSOCK: strFileType = "socket";                       break;
        default:       strFileType = "unknown?";                     break;
    }
    cout << pathStr <<"'s type is \""<< strFileType << "\" and take up "<<fileSize<<" bytes"<<endl;
}
