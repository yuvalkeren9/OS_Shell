#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <vector>
//#include <string.h>
#include <time.h>

const std::string WHITESPACE = " \n\r\t\f\v";

#define COMMAND_ARGS_MAX_LENGTH (400)
#define COMMAND_MAX_ARGS (30)

std::string _ltrim(const std::string& s);
std::string _rtrim(const std::string& s);
std::string _trim(const std::string& s);
int _parseCommandLine(const char* cmd_line, char** args);
bool _isBackgroundComamnd(const char* cmd_line);
bool isSpecialExternalCommand(const char* cmd_line);
void _removeBackgroundSign(char* cmd_line);
char** makeArgsArr(const char *cmd_line, char* first_word);



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
        delete previous;
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
public:
  QuitCommand(const char* cmd_line, JobsList* jobs);
  virtual ~QuitCommand() {}
  void execute() override;
};


class JobsList {
 public:
  class JobEntry {
   // TODO: Add your data members
  private:
   int jobID;
   pid_t pid;
   const ExternalCommand& command;
   time_t* jobTime;
   bool stopped;

  public:
      JobEntry(int jobID, pid_t pid, const ExternalCommand& command , bool stopped);
      ~JobEntry();
      void printJob();
      int getJobID() const;

  };
 // TODO: Add your data members
 public:
  JobsList();
  ~JobsList()= default;
  void addJob(ExternalCommand* cmd,pid_t pid,  bool isStopped = false);
  void printJobsList();
  void killAllJobs();
  void removeFinishedJobs();
  JobEntry * getJobById(int jobId);
  void removeJobById(int jobId);
  JobEntry * getLastJob(int* lastJobId);
  JobEntry *getLastStoppedJob(int *jobId);
  int getLargestJobID();

 private:
    int numOfJobs;
    std::vector<JobEntry*>  jobsVector ;
  // TODO: Add extra methods or modify exisitng ones as needed
};

class JobsCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  JobsCommand(const char* cmd_line, JobsList* jobs);
  virtual ~JobsCommand() {}
  void execute() override;
};

class ForegroundCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  ForegroundCommand(const char* cmd_line, JobsList* jobs);
  virtual ~ForegroundCommand() {}
  void execute() override;
};

class BackgroundCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  BackgroundCommand(const char* cmd_line, JobsList* jobs);
  virtual ~BackgroundCommand() {}
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
 // TODO: Add your data members
 public:
  KillCommand(const char* cmd_line, JobsList* jobs);
  virtual ~KillCommand() {}
  void execute() override;
};

class SmallShell {
 private:
    char* previousPath;
    std::string shellPromt;
    JobsList jobList;
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
  std::string getPromt() const;

};


//garbage


#endif //SMASH_COMMAND_H_
