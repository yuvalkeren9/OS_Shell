#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <utility>
#include <vector>
//#include <string.h>
#include <time.h>

using std::string;

const std::string WHITESPACE = " \n\r\t\f\v";

#define COMMAND_ARGS_MAX_LENGTH (400)
#define COMMAND_MAX_ARGS (30)

std::string _ltrim(const std::string& s);
std::string _rtrim(const std::string& s);
std::string _trim(const std::string& s);
int _parseCommandLine(const char* cmd_line, char** args);
bool _isBackgroundComamnd(const char* cmd_line);
bool isSpecialExternalCommand(const char* cmd_line);
string removeTimeoutAndFriends(const char* cmd_line);

bool isTimeoutCommand(const char* cmd_line);
void _removeBackgroundSign(char* cmd_line);
char** makeArgsArr(const char *cmd_line);
string cutUntillChar(const char* toCut , char character);
string cutAfterChar(const char* cmd_line , char character);


char* removeMinusFromStartOfString(char *str);
long convertStringToInt(char* str);
int removeMinusFromStringAndReturnAsInt(char* str);
long getTimeOfAlaram(const char* cmd_line);


int findFirstCharInArgs(const string& str, char** arguments, int numOfArgs);
int getCrocLocation(char** arguments, int numberOfWords);
int getPipeLocation(char** arguments, int numberOfWords);






class Command {


protected:
    const char* cmd_line;
public:
  Command(const char* cmd_line);
  virtual ~Command() {};
  virtual void execute() = 0;
  const char* getCommand() const;
  //virtual void prepare();
  //virtual void cleanup();
  // TODO: Add your extra methods if needed
};

class BuiltInCommand : public Command {
 public:
  BuiltInCommand(const char* cmd_line);
  virtual ~BuiltInCommand() {}
};

class ExternalCommand : public Command {
 public:
  ExternalCommand(const char* cmd_line);
  virtual ~ExternalCommand() {}
  void execute() override;
};

class PipeCommand : public Command {
  // TODO: Add your data members
 public:
  PipeCommand(const char* cmd_line);
  virtual ~PipeCommand() {}
  void execute() override;
};

class RedirectionCommand : public Command {
 // TODO: Add your data members
 public:
  explicit RedirectionCommand(const char* cmd_line);
  virtual ~RedirectionCommand() {}
  void execute() override;
  //void prepare() override;
  //void cleanup() override;
};

class ChangeDirCommand : public BuiltInCommand {
// TODO: Add your data members public:
public:
    ChangeDirCommand(const char *cmd_line, char **plastPwd, char*& previous);
    virtual ~ChangeDirCommand() {
//        delete previous;
    }
    void execute() override;
private:
    char **plastPwd;
    char*& previous;
};

class GetCurrDirCommand : public BuiltInCommand {
 public:
  GetCurrDirCommand(const char* cmd_line);
  virtual ~GetCurrDirCommand() {}
  void execute() override;
};

class ChPromtCommand : public BuiltInCommand {
private:
    std::string& prompt;
public:
    ChPromtCommand(const char* cmd_line, std::string& prompt);
    virtual ~ChPromtCommand() {}
    void execute() override;
};

class ShowPidCommand : public BuiltInCommand {
 public:
  ShowPidCommand(const char* cmd_line);
  virtual ~ShowPidCommand() {}
  void execute() override;
};

class JobsList;
class QuitCommand : public BuiltInCommand {
// TODO: Add your data members
    JobsList* jobs;
public:
  QuitCommand(const char* cmd_line, JobsList* jobs);
  virtual ~QuitCommand() {}
  void execute() override;
};

class TimeoutCommand : public BuiltInCommand {
/* Bonus */
// TODO: Add your data members
public:
    explicit TimeoutCommand(const char* cmd_line);
    virtual ~TimeoutCommand() {}
    void execute() override;
};

class TimeoutEntry{
public:
    pid_t pid;
    long timeToRun;
    string cmdString;
    time_t* timeEntry;
    TimeoutEntry(pid_t pid, long timeToRun, string cmdString) : pid(pid), timeToRun(timeToRun), cmdString(cmdString){
            time_t* temptime = new ::time_t ;
            time(temptime);
            timeEntry = temptime;
    }
    ~TimeoutEntry(){
        delete timeEntry;
    }
};


class JobsList {
 public:
  class JobEntry {
   // TODO: Add your data members
  private:
   int jobID;
   pid_t pid;
   string cmd_line;
   time_t* jobTime;
   bool stopped;

  public:
      JobEntry(int jobID, pid_t pid,string cmd_line , bool stopped);
      ~JobEntry();
      void printJob() const;
      void printJob_for_fg() const;
      void printJobForKillAll() const;
      int getJobID() const;
      pid_t getJobPID() const;
      std::string get_cmd_line() const;
      void updateJobStoppedStatus();
      bool isStopped() const;
      void resetTime();


  };
 // TODO: Add your data members
 public:
  JobsList();
  ~JobsList()= default;
  void addJob(const char* cmd_line,pid_t pid,  bool isStopped = false);
  void printJobsList();
  void killAllJobs();
  void removeFinishedJobs();
  JobEntry * getJobById(int jobId);
  void removeJobById(int jobId);
  int removeJobByPID(int jobPID);
  JobEntry * getLastJob(int* lastJobId);
  JobEntry *getLastStoppedJob(int *jobId);
  int getLargestJobID() const;
  int getLargestStoppedJobID() const;
  bool isEmpty() const;
  int getNumOfJobs() const;
  JobsList::JobEntry* getJobByPID(pid_t jobPID) const;
  void resetJobTime(pid_t pid);


private:
    int numOfJobs;
    std::vector<JobEntry*>  jobsVector ;
  // TODO: Add extra methods or modify exisitng ones as needed
};

class JobsCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  JobsCommand(const char* cmd_line);
  virtual ~JobsCommand() {}
  void execute() override;
};

class ForegroundCommand : public BuiltInCommand {
    int jobId;
    JobsList* jobsList;
    static const  int badArgumentsError = -1;
    static const int emptyListError = -2;
 public:
  ForegroundCommand(const char* cmd_line, JobsList* jobs);
  virtual ~ForegroundCommand() {}
  void execute() override;
};

class BackgroundCommand : public BuiltInCommand {
    JobsList* jobList;
    int jobId;
    static const  int badArgumentsError = -1;
    static const int emptyListError = -2;
 public:
  BackgroundCommand(const char* cmd_line, JobsList* jobs);
  virtual ~BackgroundCommand() {}
  void execute() override;
};



class ChmodCommand : public BuiltInCommand {
  // TODO: Add your data members
 public:
  ChmodCommand(const char* cmd_line);
  virtual ~ChmodCommand() {}
  void execute() override;
};

class GetFileTypeCommand : public BuiltInCommand {
  // TODO: Add your data members
 public:
  GetFileTypeCommand(const char* cmd_line);
  virtual ~GetFileTypeCommand() {}
  void execute() override;
};

class SetcoreCommand : public BuiltInCommand {
  // TODO: Add your data members
 public:
  SetcoreCommand(const char* cmd_line);
  virtual ~SetcoreCommand() {}
  void execute() override;
};

class KillCommand : public BuiltInCommand {
    JobsList* jobsList;
 public:
  KillCommand(const char* cmd_line, JobsList* jobs);
  virtual ~KillCommand() {}
  void execute() override;

    static int removeMinusFromStartOfString(char *string);
};

class SmallShell {
 private:
    char* previousPath;
    std::string shellPromt;
    JobsList jobList;
    pid_t foregroundCommandPID;
    ExternalCommand* externalCommandInFgPointer;
    string ForeGround_cmd_line;
    SmallShell();
 public:
  Command *CreateCommand(const char* cmd_line);
  SmallShell(SmallShell const&)      = delete; // disable copy ctor
  void operator=(SmallShell const&)  = delete; // disable = operator
  static SmallShell& getInstance() // make SmallShell singleton
  {
    static SmallShell instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
  }
  ~SmallShell();
  void executeCommand(const char* cmd_line);
  char* getPreviousPath();
  void updatePreviousPath(char* path);
  JobsList* getJoblist();
  std::string getPromt() const;
  void updateForegroundCommandPID(pid_t pid);
  pid_t getForegroundCommandPID() const;

  std::vector<TimeoutEntry*> timeoutEntryVector;
  TimeoutEntry* getTimeoutEntryByPid(pid_t pid) const;
  void eraseTimeoutEntryByPid(pid_t pid);

  ExternalCommand* getExternalCommandInFgPointer() const;
  void setExternalCommandInFgPointer(ExternalCommand* ptr);
  BuiltInCommand* checkCmdForBuiltInCommand(const char* cmd_line);


    string get_fg_cmd_line() const {
      return ForeGround_cmd_line;
  }

  void update_fg_cmd_line(string cmd_line){
      ForeGround_cmd_line = cmd_line;
  }

    void reap();
};


//garbage


#endif //SMASH_COMMAND_H_
