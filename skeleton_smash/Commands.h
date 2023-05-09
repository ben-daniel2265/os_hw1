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
  time_t endTime;
  public:
  Command(const char* cmd_line, time_t endTime) {this->cmd_line = cmd_line;
                                                this->endTime = endTime;}
  virtual ~Command() {}
  virtual void execute() = 0;
  //virtual void prepare();
  //virtual void cleanup();
  virtual bool isExternal() = 0;
  const char* getCmdLine() {return cmd_line;}
  void setEndTime(time_t endTime) {this->endTime = endTime;}
  time_t getEndTime() {return endTime;}
};

class BuiltInCommand : public Command {
 public:
  BuiltInCommand(const char* cmd_line, time_t endTime) : Command(cmd_line, endTime) {}
  virtual ~BuiltInCommand() {}
  bool isExternal() override { return false; }
};

class ExternalCommand : public Command {
 public:
  ExternalCommand(const char* cmd_line, time_t endTime = NULL) : Command(cmd_line, endTime) {}
  virtual ~ExternalCommand() {}
  void execute() override;
  bool isExternal() override { return true; }
};

class PipeCommand : public Command {
  // TODO: Add your data members
 public:
  PipeCommand(const char* cmd_line, time_t endTime = NULL) : Command(cmd_line, endTime) {}
  virtual ~PipeCommand() {}
  void execute() override;
  bool isExternal() override { return false; }
};

class RedirectionCommand : public Command {
 // TODO: Add your data members
  public:
  explicit RedirectionCommand(const char* cmd_line, time_t endTime = NULL) : Command(cmd_line, endTime) {}
  virtual ~RedirectionCommand() {}
  void execute() override;
  bool isExternal() override { return false; }
  //void prepare() override;
  //void cleanup() override;
};

class ChpromptCommand : public BuiltInCommand {
  public:
  ChpromptCommand(const char* cmd_line, time_t endTime = NULL) : BuiltInCommand(cmd_line, endTime) {}
  virtual ~ChpromptCommand() {}
  void execute() override;
};

class ChangeDirCommand : public BuiltInCommand {
// TODO: Add your data members 
  public:
  ChangeDirCommand(const char* cmd_line, time_t endTime = NULL) : BuiltInCommand(cmd_line, endTime) {}
  virtual ~ChangeDirCommand() {}
  void execute() override;
};

class GetCurrDirCommand : public BuiltInCommand {
 public:
  GetCurrDirCommand(const char* cmd_line, time_t endTime = NULL) : BuiltInCommand(cmd_line, endTime) {}
  virtual ~GetCurrDirCommand() {}
  void execute() override;
};

class ShowPidCommand : public BuiltInCommand {
 public:
  ShowPidCommand(const char* cmd_line, time_t endTime = NULL) : BuiltInCommand(cmd_line, endTime) {}
  virtual ~ShowPidCommand() {}
  void execute() override;
};


class JobsList;
class QuitCommand : public BuiltInCommand {
  JobsList* jobs;
public:
  QuitCommand(const char* cmd_line, time_t endTime = NULL) : BuiltInCommand(cmd_line, endTime) {}
  virtual ~QuitCommand() {}
  void execute() override;
};


class JobsList {
  public:
  int maxJobId;

  class JobEntry {
    int jobId;
    bool isStopped;
    char* cmd;
    time_t startTime;
    pid_t jobPid;

    public:
    JobEntry(bool isStopped, char* cmd, JobsList* jobList, pid_t jobPid) : isStopped(isStopped), cmd(cmd), jobPid(jobPid) 
    {
      this->jobId = jobList->maxJobId + 1;
      jobList->maxJobId = this->jobId;
      startTime = time(NULL);
    }

    int getJobId() {return jobId;}
    bool getIsStopped() {return isStopped;}
    char* getCmd() {return cmd;}
    time_t getJobTime() {return startTime;}
    
    void setIsStopped(bool isStopped) {this->isStopped = isStopped;}
    void setCmd(char* cmd) {this->cmd = cmd;}
    void setJobId(int jobId) {this->jobId = jobId;}
    void setJobTime(time_t jobTime) {this->startTime = jobTime;}

    pid_t getJobPid() {return this->jobPid;}

  };

  private:
  vector<JobEntry*> jobs;

  public:
  JobsList() {maxJobId = 0;}
  ~JobsList();
  void addJob(char* cmd, JobsList* jobList, pid_t jobPid, bool isStopped);
  void addJob(JobEntry* job);
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
 public:
  JobsCommand(const char* cmd_line, time_t endTime = NULL) : BuiltInCommand(cmd_line, endTime) {}
  virtual ~JobsCommand() {}
  void execute() override;
};

class ForegroundCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  ForegroundCommand(const char* cmd_line, time_t endTime = NULL) : BuiltInCommand(cmd_line, endTime) {}
  virtual ~ForegroundCommand() {}
  void execute() override;
};

class BackgroundCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  BackgroundCommand(const char* cmd_line, time_t endTime = NULL) : BuiltInCommand(cmd_line, endTime) {}
  virtual ~BackgroundCommand() {}
  void execute() override;
};

class TimeoutCommand : public BuiltInCommand {
/* Bonus */
// TODO: Add your data members
 public:
  explicit TimeoutCommand(const char* cmd_line, time_t endTime = NULL) : BuiltInCommand(cmd_line, endTime) {}
  virtual ~TimeoutCommand() {}
  void execute() override;
};

class ChmodCommand : public BuiltInCommand {
  // TODO: Add your data members
 public:
  ChmodCommand(const char* cmd_line, time_t endTime = NULL) : BuiltInCommand(cmd_line, endTime) {}
  virtual ~ChmodCommand() {}
  void execute() override;
};

class GetFileTypeCommand : public BuiltInCommand {
  // TODO: Add your data members
 public:
  GetFileTypeCommand(const char* cmd_line, time_t endTime = NULL) : BuiltInCommand(cmd_line, endTime) {}
  virtual ~GetFileTypeCommand() {}
  void execute() override;
};

class SetcoreCommand : public BuiltInCommand {
  // TODO: Add your data members
 public:
  SetcoreCommand(const char* cmd_line, time_t endTime = NULL) : BuiltInCommand(cmd_line, endTime) {}
  virtual ~SetcoreCommand() {}
  void execute() override;
};

class KillCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  KillCommand(const char* cmd_line, time_t endTime = NULL) : BuiltInCommand(cmd_line, endTime) {};
  virtual ~KillCommand() {}
  void execute() override;
};

class SmallShell {
 private:
  char* prompt;
  char* lastPwd;
  JobsList* jobs;
  JobsList::JobEntry* fg_job;
  pid_t smashPid;

  struct TimedCommand {
    Command* cmd;
    pid_t pid;
  } typedef TimedCommand;

  vector<TimedCommand*> timedJobs;

  SmallShell(){
                this->prompt = new char[6];
                strcpy(this->prompt, "smash");
                this->lastPwd = nullptr;
                this->jobs = new JobsList();
                this->fg_job = NULL;
                this->smashPid = getpid();
                }
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

  pid_t getSmashPid() {return this->smashPid;}

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

  JobsList* getJobsList() {return this->jobs;}

  JobsList::JobEntry* getFgJob() {return this->fg_job;}
  void setFgJob(JobsList::JobEntry* job) {this->fg_job = job;}
  void addTimedJob(Command* cmd, pid_t pid);
  void resetAlarm();
};

#endif //SMASH_COMMAND_H_
