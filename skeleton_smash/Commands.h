#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <vector>
#include <time.h>
#include <string.h>

#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)

using namespace std;
//const char* DEFAULT_PROMPT = "smash";
class Command {
// TODO: Add your data members
  const char* cmd_line;
  public:
  Command(const char* cmd_line) {this->cmd_line = cmd_line;}
  virtual ~Command() {}
  virtual void execute() = 0;
  //virtual void prepare();
  //virtual void cleanup();
  virtual bool isExternal() = 0;
  const char* getCmdLine() {return cmd_line;}
};

class BuiltInCommand : public Command {
 public:
  BuiltInCommand(const char* cmd_line) : Command(cmd_line) {}
  virtual ~BuiltInCommand() {}
  bool isExternal() override { return false; }
};

class ExternalCommand : public Command {
 public:
  ExternalCommand(const char* cmd_line) : Command(cmd_line) {}
  virtual ~ExternalCommand() {}
  void execute() override {}
  bool isExternal() override { return true; }
};

class PipeCommand : public Command {
  // TODO: Add your data members
 public:
  PipeCommand(const char* cmd_line) : Command(cmd_line) {}
  virtual ~PipeCommand() {}
  void execute() override;
};

class RedirectionCommand : public Command {
 // TODO: Add your data members
  public:
  explicit RedirectionCommand(const char* cmd_line) : Command(cmd_line) {}
  virtual ~RedirectionCommand() {}
  void execute() override;
  //void prepare() override;
  //void cleanup() override;
};

class ChpromptCommand : public BuiltInCommand {
  public:
  ChpromptCommand(const char* cmd_line) : BuiltInCommand(cmd_line) {}
  virtual ~ChpromptCommand() {}
  void execute() override;
};

class ChangeDirCommand : public BuiltInCommand {
// TODO: Add your data members 
  public:
  ChangeDirCommand(const char* cmd_line) : BuiltInCommand(cmd_line) {}
  virtual ~ChangeDirCommand() {}
  void execute() override;
};

class GetCurrDirCommand : public BuiltInCommand {
 public:
  GetCurrDirCommand(const char* cmd_line) : BuiltInCommand(cmd_line) {}
  virtual ~GetCurrDirCommand() {}
  void execute() override;
};

class ShowPidCommand : public BuiltInCommand {
 public:
  ShowPidCommand(const char* cmd_line) : BuiltInCommand(cmd_line) {}
  virtual ~ShowPidCommand() {}
  void execute() override;
};

class JobsList;
class QuitCommand : public BuiltInCommand {
  JobsList* jobs;
public:
  QuitCommand(const char* cmd_line, JobsList* jobs) : BuiltInCommand(cmd_line) {this->jobs = jobs;}
  virtual ~QuitCommand() {}
  void execute() override;
};


class JobsList {
  public:
  int maxJobId;
  class JobEntry {
   // TODO: Add your data members
    int jobId;
    bool isStopped;
    Command* cmd;
    time_t startTime;

    public:
    JobEntry(bool isStopped, Command* cmd, JobsList* jobList) : isStopped(isStopped), cmd(cmd) 
    {
      this->jobId = jobList->maxJobId + 1;
      jobList->maxJobId = this->jobId;
      startTime = time(NULL);
    }

    int getJobId() {return jobId;}
    bool getIsStopped() {return isStopped;}
    Command* getCmd() {return cmd;}
    time_t getJobTime() {return startTime;}
    
    void setIsStopped(bool isStopped) {this->isStopped = isStopped;}
    void setCmd(Command* cmd) {this->cmd = cmd;}
    void setJobId(int jobId) {this->jobId = jobId;}

    pid_t getJobPid() {}

  };
  private:
  vector<JobEntry*> jobs;
 // TODO: Add your data members
  public:
  JobsList() {}
  ~JobsList();
  void addJob(Command* cmd, JobsList* jobList, bool isStopped = false);
  void printJobsList();
  void killAllJobs();
  void removeFinishedJobs();
  JobEntry * getJobById(int jobId);
  void removeJobById(int jobId);
  JobEntry * getLastJob(int* lastJobId);
  JobEntry *getLastStoppedJob(int *jobId);
  // TODO: Add extra methods or modify exisitng ones as needed
};

class JobsCommand : public BuiltInCommand {
 // TODO: Add your data members
  JobsList* jobs;
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
  char* prompt;
  char* lastPwd;
  SmallShell(){
                this->prompt = new char[6];
                strcpy(this->prompt, "smash");
                this->lastPwd = nullptr;}
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
  char* getPrompt() {return this->prompt;}
  void setPrompt(const char* newPrompt) {   delete this->prompt;
                                            this->prompt = new char[strlen(newPrompt) + 1];
                                            strcpy(this->prompt, newPrompt);}
  char* getLastPwd() {return this->lastPwd;}
  void setLastPwd(const char* newLastPwd) { delete this->lastPwd;
                                            this->lastPwd = new char[strlen(newLastPwd) + 1];
                                            strcpy(this->lastPwd, newLastPwd);}
  // TODO: add extra methods as needed
};

#endif //SMASH_COMMAND_H_
