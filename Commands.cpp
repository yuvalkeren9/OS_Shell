
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
#include <sched.h>


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


bool isTimeoutCommand(const char* cmd_line){
    assert(cmd_line != nullptr);
    //annoying garbage
    char** arguments = new char*[COMMAND_MAX_ARGS];
    int numberOfWords = _parseCommandLine(cmd_line, arguments);
    string cmd_s = _trim(string(cmd_line));
    string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));

    delete[] arguments;  //maybe delete
    if (firstWord == "timeout"){
        return true;
    }
    else{
        return false;
    }
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

string cutAfterChar2(const char* cmd_line , char character){// like cutAfterChar, but searches from the end
    string str;
    int firstAppearIndex = (string(cmd_line).find_last_of(character));
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



long convertStringToInt(char* str){
    int temp;
    try{
        temp = stoi(string(str));
    }
    catch(std::exception& e){
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
        if (num == -1) { //error has occured
            return -1;
        }
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
        if (string(arguments[i]).find(str) != string::npos ){
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
            return;
        }
        else if ( pid == -1){
            return;
        }
        auto jobToRemove = jobList.getJobByPID(pid);
        if(jobToRemove == nullptr){

            return;
        }
        eraseTimeoutEntryByPid(pid);
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

    if (firstWord == "showpid") {
        return new ShowPidCommand(cmd_line_edit);
    }
    else if(firstWord == "pwd"){
        return new GetCurrDirCommand(cmd_line_edit);
    }
    else if(firstWord == "cd"){
        return new ChangeDirCommand(cmd_line_edit, arguments, previousPath);
    }
    else if(firstWord == "chprompt"){
        return new ChPromtCommand(cmd_line_edit, shellPromt);
    }
    else if(firstWord == "jobs"){
        return new JobsCommand(cmd_line_edit);
    }
    else if(firstWord == "fg"){
        return new ForegroundCommand(cmd_line_edit, &jobList );
    }
    else if(firstWord == "kill"){
        return new KillCommand(cmd_line_edit, &jobList);
    }
    else if(firstWord == "bg"){
        return new BackgroundCommand(cmd_line_edit, &jobList);
    }
    else if(firstWord == "quit"){
        return new QuitCommand(cmd_line_edit, &jobList);
    }
    else if(firstWord == "getfiletype"){
        return new GetFileTypeCommand(cmd_line_edit);
    }
    else if(firstWord == "chmod"){
        return new ChmodCommand(cmd_line_edit);
    }
    else if(firstWord == "setcore"){
        return new SetcoreCommand(cmd_line_edit);
    }
    else if(firstWord == "timeout"){
        return new TimeoutCommand(cmd_line);
    }
    else{
        return nullptr;
    }
}

TimeoutEntry *SmallShell::getTimeoutEntryByPid(pid_t pid) const {
    auto& smashy = SmallShell::getInstance();

    for(TimeoutEntry* entry : smashy.timeoutEntryVector){
        if (entry->pid ==   pid){
            return entry;
        }
    }
    return nullptr;   //if was not found
}

void SmallShell::eraseTimeoutEntryByPid(pid_t pid) {
    auto& smashy = SmallShell::getInstance();
    int i = 0;
    for (TimeoutEntry* entry : smashy.timeoutEntryVector){
        if (entry->pid == pid){
            smashy.timeoutEntryVector.erase(smashy.timeoutEntryVector.begin() + i);
        }
        ++i;
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
    cout <<"smash pid is "<< getpid() << endl;   //getpid is always succsesfull
}



/**-------------------------------------------------- pwd command  ----------------------------------------------------------   */

GetCurrDirCommand::GetCurrDirCommand(const char *cmd_line) : BuiltInCommand(cmd_line) {

}

void GetCurrDirCommand::execute() {
    char buffer[COMMAND_ARGS_MAX_LENGTH];
    char* message = getcwd(buffer, COMMAND_ARGS_MAX_LENGTH);
    if (message == NULL){
        perror("smash error: getcwd failed");
        return;
    }
    cout << message << endl;
}




/**-------------------------------------------------- Quit command  ----------------------------------------------------------   */

QuitCommand::QuitCommand(const char *cmd_line, JobsList *jobs): BuiltInCommand(cmd_line),jobs(jobs) {
}

void QuitCommand::execute() {
    SmallShell& smashy = SmallShell::getInstance();
//    char** arguments= makeArgsArr(cmd_line);
//    string str = arguments[0];
    char *arguments[COMMAND_MAX_ARGS];
    int numberOfWords = _parseCommandLine(cmd_line, arguments);

    if (arguments[1] != NULL){
        string str(arguments[1]);
        if(str.compare("kill")==0) {
            cout << "smash: sending SIGKILL signal to " << smashy.getJoblist()->getNumOfJobs() << " jobs:" << endl;
            jobs->killAllJobs();
        }
    }
    sleep(1);
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

    int crocType = checkCrocType(cmd_line);



    enum RedirectionType {OneCrocodile, TwoCrocodile, Other};
    RedirectionType redirectionType;
    switch (crocType){
        case 1:    //"<"
            redirectionType = OneCrocodile;
            break;
        case 2:    //">>"
            redirectionType = TwoCrocodile;
            break;
        default:
            redirectionType = Other;
            break;
    }
    if (redirectionType == RedirectionType::Other){
        cout << "Invalid arguments thing" << endl;
        //TODO: real error message
        return;
    }

    //so we can put std::cout back in FDT after we are done with redirection
    int stdout_fd = dup(1);
    if(stdout_fd == SYSCALL_FAILED){
        perror("smash error: dup failed");
    }


   int fileToOpenIndex = crocLocationIndex + 1;
   int fd_of_file = 0;

    string temp = cutAfterChar2(cmd_line, '>');
    temp = _trim(temp);
    const char* cmd_line_after_redirection_letter = temp.c_str();

   switch (redirectionType){
        case OneCrocodile:
           fd_of_file = open(cmd_line_after_redirection_letter,O_CREAT | O_RDWR |O_TRUNC,  S_IRUSR | S_IWUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

            if( fd_of_file == SYSCALL_FAILED){
                perror("smash error: open failed");
            }
            break;
        case TwoCrocodile:
            fd_of_file = open(cmd_line_after_redirection_letter,O_CREAT | O_RDWR | O_APPEND,  S_IRUSR | S_IWUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

            if (fd_of_file == SYSCALL_FAILED){
                perror("smash error: open failed");
            }
            //TODO: figure ou what positions to give
            break;
        default:
            cout << "if im here, something has gone terribly wrong.";
            break;
   }
   if (fd_of_file == SYSCALL_FAILED){
       if(dup2(stdout_fd,1) == SYSCALL_FAILED){  //return to previous state
           perror("smash error: dup2 failed");
       }
       return;
   }

    if(dup2(fd_of_file,1) == SYSCALL_FAILED){
        perror("smash error: dup2 failed");
    }

    commandToExecutre->execute();


    if(dup2(stdout_fd,1) == SYSCALL_FAILED){
        perror("smash error: dup2 failed");
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
    string temp2 = cutAfterChar(cmd_lineasfasf, letter); //TODO: check here for a bug - maybe check 'last of"?
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
            if( dup2(stderror_fd, STDERROR_FDT) == -1){
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
        perror("smash error: close failed");
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
        cerr << "smash error: getfiletype: invalid arguments" << std::endl;
        return;
    }

    if(arguments[1] == NULL){
        cerr << "smash error: getfiletype: invalid arguments" << std::endl;
        return;
    }





    const char *path = arguments[1];
    string pathStr =string (path);
    struct stat fileStats;
    if(stat(path,&fileStats)==SYSCALL_FAILED){
        perror("smash error: stat failed");
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
    cout << pathStr <<"'s type is \""<< strFileType << "\" and takes up "<<fileSize<<" bytes"<<endl;
}

ChmodCommand::ChmodCommand(const char *cmd_line) : BuiltInCommand(cmd_line) {

}

void ChmodCommand::execute() {
    //constats declarations for chmod command

    static const int mode_arg_index = 1;
    static const int path_arg_index = 2;
    static const int last_arg_index = 2;

    char *arguments[COMMAND_MAX_ARGS];
    int numberOfWords = _parseCommandLine(cmd_line, arguments);






    if (arguments[last_arg_index + 1] != NULL){
        cerr << "smash error: chmod: invalid arguments" << endl;
        return;
    }

    if (arguments[last_arg_index] == NULL){
        cerr << "smash error: chmod: invalid arguments" << endl;
        return;
    }

    int num_entered_by_user = convertStringToInt(arguments[mode_arg_index]);
    if (num_entered_by_user == -1){  //error has occured
        cerr << "smash error: chmod: invalid arguments" << endl;
        return;
    }

    //TODO: this fucntion cant revive some stuff' make sure to check on piaza (number like 9 cant be converted to oct)
    mode_t mode = std::stoi(arguments[mode_arg_index], 0 ,8);
    if(chmod(arguments[path_arg_index], mode) == -1){
        perror("smash error: chmod failed");
    }
}


SetcoreCommand::SetcoreCommand(const char *cmd_line) : BuiltInCommand(cmd_line) {

}

void SetcoreCommand::execute() {
    auto& smashy = SmallShell::getInstance();
    char *arguements[COMMAND_MAX_ARGS];
    int numberOfWords = _parseCommandLine(cmd_line, arguements);

    static const int jobIdIndex = 1;
    static const int coreIndex = 2;
    static const int lastArgIndex = 2;

    if (arguements[lastArgIndex + 1] != NULL){
        cerr << "smash error: setcore: invalid arguments" << endl;
        return;
    }

    int temp;
    try{
        temp = stoi(string(arguements[jobIdIndex]));
    }
    catch(std::exception& e){
        cerr << "smash error: setcore: invalid arguments" << endl;
        return;
    }
    int jobId = temp;

    try{
        temp = stoi(string(arguements[coreIndex]));
    }
    catch(std::exception& e){
        cerr << "smash error: setcore: invalid arguments" << endl;
        return;
    }
    int coreNumber = temp;

    long numOfTotalCores = sysconf(_SC_NPROCESSORS_CONF);
    if (numOfTotalCores == SYSCALL_FAILED) {
        perror("smash error: sysconf failed");
        return;
    }

    if (coreNumber > numOfTotalCores ||  coreNumber < 0) {
        cerr << "smash error: setcore: invalid core number" << endl;
        return;
    }


    //get the job (including if it is even in the list
    JobsList* jobList = smashy.getJoblist();
    JobsList::JobEntry* jobEntry = jobList->getJobById(jobId);
    if (jobEntry == nullptr){
        cerr << "smash error: setcore: job-id "<< jobId <<" does not exist" << endl;
        return;
    }

    pid_t pid = jobEntry->getJobPID();
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(coreNumber, &set);

    if ((sched_setaffinity(pid, sizeof(set), &set)) == SYSCALL_FAILED) {
        perror("smash error: sched_setaffinity failed");
        return;
    }
}






void TimeoutCommand::execute() {
    auto& smashy = SmallShell::getInstance();
    bool isBackground = _isBackgroundComamnd(cmd_line);
    bool isSpecialCommand = isSpecialExternalCommand(cmd_line);


    char* checkForSecondArg[COMMAND_MAX_ARGS];
    _parseCommandLine(cmd_line, checkForSecondArg);
    if (checkForSecondArg[2] == NULL ){
        cerr << "smash error: timeout: invalid arguments" << endl; //maybe not neccesary
        return;
    }

    //edit the cmd_line
    string temp = removeTimeoutAndFriends(cmd_line);
    const char* cmd_line_edit_temp = temp.c_str();
    char cmd_line_edit[COMMAND_ARGS_MAX_LENGTH];
    strcpy(cmd_line_edit, cmd_line_edit_temp );

    if (isBackground){
        _removeBackgroundSign(cmd_line_edit);
    }

    char* arguments[COMMAND_MAX_ARGS];
    int numberOfWords = _parseCommandLine(cmd_line_edit, arguments);



    long timeOfAlarm = getTimeOfAlaram(cmd_line);
    if (timeOfAlarm <= 0){
        cerr << "smash error: timeout: invalid arguments" << endl; //maybe not neccesary
        return;
    }



    // piaza said that built in can also work, so need to do that, and it also said we can assume that builtin will finish beofre, so no need to add to vector, or even fork
    BuiltInCommand* builtInCommand = smashy.checkCmdForBuiltInCommand(cmd_line_edit);
    if (builtInCommand != nullptr){    //someone ran timeout command with builtin for some reason.. wtf?
        alarm(timeOfAlarm);
        builtInCommand->execute();
        delete builtInCommand;
        return;
    }

    pid_t pid = fork();
    if (pid == 0) {                                                         //child
        setpgrp();
        if (!isSpecialCommand){
            if (execv(arguments[0], arguments) == -1){         //TODO: update the correct arguments
                if (execvp(arguments[0],arguments)==-1)
                {
//                    perror("smash error: execv failed");
                    perror(cmd_line);  //TODO: change this motherfucker back to the above line

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



    else {                                                          //parent
        alarm(timeOfAlarm); //todo perror?
        smashy.timeoutEntryVector.push_back(new TimeoutEntry(pid, timeOfAlarm , cmd_line));
        if (isBackground){
            smashy.getJoblist()->addJob(cmd_line,pid,false);
        }
        else {
            int status;
            smashy.update_fg_cmd_line(cmd_line);
            smashy.updateForegroundCommandPID(pid);
            waitpid(pid, &status, WUNTRACED);
            if (WIFSTOPPED(status) == false){     //child was terminated, either by signal or regular but was not stopped
                smashy.eraseTimeoutEntryByPid(pid);
            }
            smashy.updateForegroundCommandPID(0);
            smashy.update_fg_cmd_line("");  //maybe bug?
        }
    }


}

TimeoutCommand::TimeoutCommand(const char *cmd_line) : BuiltInCommand(cmd_line) {

}



string removeTimeoutAndFriends(const char* cmd_line){
    char** arguments = new char*[COMMAND_MAX_ARGS];
    int numOfArgs = _parseCommandLine(cmd_line, arguments);

    //create the string
    string str;
    for (int i = 2; i < numOfArgs ; ++i){
        str += arguments[i];
        if (i == numOfArgs - 1){
            continue;
        }
        str += " ";
    }
    return str;
}

long getTimeOfAlaram(const char* cmd_line){
    char* arguments[COMMAND_MAX_ARGS];
    _parseCommandLine(cmd_line, arguments);
    return convertStringToInt(arguments[1]);
}


int checkCrocType(const string& cmd_line){
    if ( cmd_line.find(">>") != string::npos ){
        return 2;
    }
    else if (cmd_line.find('>') != string::npos ){
        return 1;
    }
    else{
        return -1;
    }
}
