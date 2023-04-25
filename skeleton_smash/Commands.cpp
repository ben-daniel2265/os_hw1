#ifndef SMASH__COMMANDS_C_
#define SMASH__COMMANDS_C_
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"


using namespace std;

const std::string WHITESPACE = " \n\r\t\f\v";


#if 0
#define FUNC_ENTRY()  \
  cout << __PRETTY_FUNCTION__ << " --> " << endl;

#define FUNC_EXIT()  \
  cout << __PRETTY_FUNCTION__ << " <-- " << endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif

string _ltrim(const std::string& s)
{
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? "" : s.substr(start);
}

string _rtrim(const std::string& s)
{
  size_t end = s.find_last_not_of(WHITESPACE);
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

string _trim(const std::string& s)
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

bool _isComplexCommand(const char* cmd_line) {
  const string str(cmd_line);
  return ((str.find('*') != string::npos) || (str.find('?') != string::npos));
}
// TODO: Add your implementation for classes in Commands.h 

SmallShell::~SmallShell() {
// TODO: add your implementation
}

/**
* Creates and returns a pointer to Command class which matches the given command line (cmd_line)
*/
Command * SmallShell::CreateCommand(const char* cmd_line) {
  // For example:

  string cmd_s = _trim(string(cmd_line));
  string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));

  if (firstWord.compare("pwd") == 0) {
      return new GetCurrDirCommand(cmd_line);
  }
  else if (firstWord.compare("showpid") == 0) {
    return new ShowPidCommand(cmd_line);
  }
  else if (firstWord.compare("chprompt") == 0) {
    return new ChpromptCommand(cmd_line);
  }
  else if (firstWord.compare("cd") == 0) {
    return new ChangeDirCommand(cmd_line);
  }
  else if(firstWord.compare("jobs") == 0) {
    return new JobsCommand(cmd_line);
  }
  else if(firstWord.compare("fg") == 0) {
      return new ForegroundCommand(cmd_line);
  }
  else if(firstWord.compare("bg") == 0) {
      return new BackgroundCommand(cmd_line);
  }
  else if(firstWord.compare("quit") == 0) {
      return new QuitCommand(cmd_line);
  }
  else if(firstWord.compare("kill") == 0) {
      return new KillCommand(cmd_line);
  }
  else {
    return new ExternalCommand(cmd_line);
  }
  return nullptr;
}

void SmallShell::executeCommand(const char *cmd_line) {
  // TODO: Add your implementation here
  // for example:
  // Command* cmd = CreateCommand(cmd_line);
  // cmd->execute();
  // Please note that you must fork smash process for some commands (e.g., external commands....)
  
  this->jobs->removeFinishedJobs();

  Command* cmd = CreateCommand(cmd_line);
  if(cmd->isExternal()) {
    cmd->execute();
  }
  else{
    cmd->execute();
  }
}

void ExternalCommand::execute() {

  SmallShell& smash = SmallShell::getInstance();
  char* args[COMMAND_MAX_ARGS];
  char* cmd_lineCopy = (char*)malloc(strlen(this->getCmdLine())+1);
  char* ogcmd = (char*)malloc(strlen(this->getCmdLine())+1);
  strcpy(cmd_lineCopy, this->getCmdLine());
  strcpy(ogcmd, this->getCmdLine());
  bool isBackground = _isBackgroundComamnd(cmd_lineCopy);

  if(isBackground)
  {
    _removeBackgroundSign(cmd_lineCopy);
  }

  int argCount = _parseCommandLine(cmd_lineCopy, args);

  pid_t pid = fork();

  if(pid == 0) {
      setpgrp();
      bool isComplex = _isComplexCommand(cmd_lineCopy);
    
    if(isComplex) {
      char* argsBash[4];
      argsBash[0] = "bash";
      argsBash[1] = "-c";
      argsBash[2] = (char*)malloc(strlen(cmd_lineCopy)+1);
      argsBash[3] = NULL;
      strcpy(argsBash[2], cmd_lineCopy);

      if(execvp("bash", argsBash) == -1)
      {
        perror("smash error: > ");
      }
    }
    else{
      if(execvp(args[0], args) == -1)
      {
        perror("smash error: > ");
      }
    }
  }
  else {
    if(!isBackground)
      {
        JobsList::JobEntry* fgjob = new JobsList::JobEntry(false, ogcmd, smash.getJobsList(), pid);
        smash.setFgJob(fgjob);
        waitpid(pid, NULL, WUNTRACED);
        smash.setFgJob(nullptr);
      }
      else{
        smash.getJobsList()->addJob(ogcmd, smash.getJobsList(), pid, false);
      }
  }
}

void ChpromptCommand::execute() {
  SmallShell& smash = SmallShell::getInstance();

  char* args[COMMAND_MAX_ARGS];
  int argCount = _parseCommandLine(this->getCmdLine(), args);

  if(argCount == 1) {
    smash.setPrompt("smash");
  }
  else {
    smash.setPrompt(args[1]);
  }
}

void GetCurrDirCommand::execute() {
  cout << getcwd(NULL, 0) << endl;
}

void ShowPidCommand::execute() {
  cout << "smash pid is " << getpid() << endl;
}

void ChangeDirCommand::execute() {
  char* args[COMMAND_MAX_ARGS];
  char* tmepdir;

  SmallShell& smash = SmallShell::getInstance();
  int argCount = _parseCommandLine(this->getCmdLine(), args);

  if(argCount == 2) {
    if(strcmp(args[1], "-") == 0) {
      if(smash.getLastPwd() == NULL) {
        perror("smash error: cd: OLDPWD not set\n");
      }
      else {
        tmepdir = getcwd(NULL, 0);
        chdir(smash.getLastPwd());
        smash.setLastPwd(tmepdir);
      }
    }
    else{
      smash.setLastPwd(getcwd(NULL, 0));
      chdir(args[1]);
    }
  }
  else if(argCount >= 2){
    perror("smash error: cd: too many arguments\n");
  }
  else{
      string start = "smash error: ";
      char* temp = new char[strlen(start.c_str()) + strlen(this->getCmdLine()) + 1];
      strcpy(temp, start.c_str());
      strcat(temp, this->getCmdLine());
      perror(temp);
  }
}

void JobsCommand::execute() {
  SmallShell& smash = SmallShell::getInstance();
  smash.getJobsList()->printJobsList();
}

bool isNumber(const char* str)
{
    for (int i = 0; i < strlen(str); i++)
    {
        if (!isdigit(str[i]))
        {
            return false;
        }
    }
    return true;
}

void ForegroundCommand::execute() {
    SmallShell& smash = SmallShell::getInstance();
    JobsList* jobs = smash.getJobsList();
    char* args[COMMAND_MAX_ARGS];
    int argCount = _parseCommandLine(this->getCmdLine(), args);
    int LastJobId = 0;
    if(argCount == 1)
    {
        JobsList::JobEntry* job = jobs->getLastJob(&LastJobId);
        if(job == nullptr)
        {
            perror("smash error: fg: jobs list is empty\n");
        }
        else {
            if (job->getIsStopped()) {
                kill(job->getJobPid(), SIGCONT);
                job->setIsStopped(false);
            }
            smash.setFgJob(job);
            jobs->removeJobById(LastJobId);
            waitpid(job->getJobPid(), NULL, WUNTRACED);
            smash.setFgJob(nullptr);

        }
    }
    else if(argCount > 2 || !isNumber(args[1]))
    {
        perror("smash error: fg: invalid arguments\n");
    }
    else
    {
        JobsList::JobEntry* job = jobs->getJobById(atoi(args[1]));
        if(job == nullptr)
        {
            char* errormsg = new char[strlen("smash error: fg: job-id ") + strlen(args[1]) + strlen(" does not exist\n") + 1];
            strcat(errormsg, "smash error: fg: job-id ");
            strcat(errormsg, args[1]);
            strcat(errormsg, " does not exist\n");
            perror(errormsg);
        }
        else {
            if (job->getIsStopped()) {
                kill(job->getJobPid(), SIGCONT);
                job->setIsStopped(false);
            }
            smash.setFgJob(job);
            jobs->removeJobById(atoi(args[1]));
            waitpid(job->getJobPid(), NULL, WUNTRACED);
            smash.setFgJob(nullptr);
        }
    }
}

void BackgroundCommand::execute() {
    SmallShell& smash = SmallShell::getInstance();
    JobsList* jobs = smash.getJobsList();
    char* args[COMMAND_MAX_ARGS];
    int argCount = _parseCommandLine(this->getCmdLine(), args);
    int lastStoppedJobId;
    JobsList::JobEntry* job = jobs->getLastStoppedJob(&lastStoppedJobId);
    if(argCount == 1)
    {
        if(job == nullptr)
        {
            perror("smash error: bg: there is no stopped jobs to resume\n");
        }
        else
        {
            kill(job->getJobPid(), SIGCONT);
            job->setIsStopped(false);
            cout << job->getCmd() << " : " << job->getJobPid() << endl;
        }
    }
    else if (argCount > 2 || !isNumber(args[1])) {
        perror("smash error: fg: invalid arguments\n");
    }
    else
    {
        job = jobs->getJobById(atoi(args[1]));
        if(job == nullptr)
        {
            char* errormsg = new char[strlen("smash error: bg: job-id ") + strlen(args[1]) + strlen(" does not exist\n") + 1];
            strcat(errormsg, "smash error: bg: job-id ");
            strcat(errormsg, args[1]);
            strcat(errormsg, " does not exist\n");
            perror(errormsg);
        }
        else
        {
            if(job->getIsStopped())
            {
                kill(job->getJobPid(), SIGCONT);
                job->setIsStopped(false);
                cout << job->getCmd() << " : " << job->getJobPid() << endl;
            }
            else
            {
                char* errormsg = new char[strlen("smash error: bg: job-id ") + strlen(args[1]) + strlen(" is already running in the background\n") + 1];
                strcat(errormsg, "smash error: bg: job-id ");
                strcat(errormsg, args[1]);
                strcat(errormsg, " is already running in the background\n");
                perror(errormsg);
            }
        }
    }
}

void QuitCommand::execute() {
    SmallShell& smash = SmallShell::getInstance();
    JobsList* jobs = smash.getJobsList();
    char* args[COMMAND_MAX_ARGS];
    int argCount = _parseCommandLine(this->getCmdLine(), args);

    if(argCount == 2 && strcmp(args[1], "kill") == 0)
    {
        jobs->killAllJobs();
    }
    exit(0);

}

void KillCommand::execute() {
    SmallShell& smash = SmallShell::getInstance();
    JobsList* jobs = smash.getJobsList();
    char* args[COMMAND_MAX_ARGS];
    int argCount = _parseCommandLine(this->getCmdLine(), args);
    JobsList::JobEntry* job = jobs->getJobById(atoi(args[2]));
    string sigNum = ((string)args[1]).substr(1);
    if(argCount != 3 || args[1][0] != '-' || !isNumber(sigNum.c_str()) || !isNumber(args[2]))
    {
        perror("smash error: kill: invalid arguments\n");
    }
    else if(job != nullptr)
    {
        if(kill(job->getJobPid(), atoi(sigNum.c_str())) != 0) {
            perror("smash error: kill failed\n");
        }
        else {
            if(atoi(sigNum.c_str()) == SIGSTOP)
            {
                cout << "bulbul "<< endl;
                job->setIsStopped(true);
            }

        }
        cout << "signal number " << sigNum << " was sent to pid " << job->getJobPid() << endl;
    }
    else
    {
        cout << "smash error: kill: job-id " << args[2] << " does not exist" << endl;
    }

}


void JobsList::removeFinishedJobs(){
    int currMaxJobId = 0;
  for (int i = 0; i < jobs.size(); i++)
  {
    if (waitpid(jobs[i]->getJobPid(), NULL, WNOHANG) == jobs[i]->getJobPid())
    {
      jobs.erase(jobs.begin() + i);
      i--;
    }
    else
    {
        if (jobs[i]->getJobId() > currMaxJobId)
        {
            currMaxJobId = jobs[i]->getJobId();
        }
    }
  }
    this->maxJobId = currMaxJobId;
}

JobsList::JobEntry * JobsList::getJobById(int jobId){
  for (int i = 0; i < jobs.size(); i++)
  {
    if (jobs[i]->getJobId() == jobId)
    {
      return jobs[i];
    }
  }
  return NULL;
}

void JobsList::removeJobById(int jobId){
    int currMaxJobId = 0;
    for (int i = 0; i < jobs.size(); ++i) {
        if(jobs[i]->getJobId() > currMaxJobId && jobs[i]->getJobId() != jobId)
        {
            currMaxJobId = jobs[i]->getJobId();
        }
    }
    this->maxJobId = currMaxJobId;

  for (int i = 0; i < jobs.size(); i++)
  {
    if (jobs[i]->getJobId() == jobId)
    {
      jobs.erase(jobs.begin() + i);
      return;
    }
  }

}

JobsList::JobEntry * JobsList::getLastJob(int* lastJobId){
  if (jobs.size() == 0)
  {
    return NULL;
  }
  *lastJobId = jobs[jobs.size() - 1]->getJobId();
  return jobs[jobs.size() - 1];
}

JobsList::JobEntry * JobsList::getLastStoppedJob(int *jobId){
  for (int i = jobs.size() - 1; i >= 0; i--)
  {
    if (jobs[i]->getIsStopped())
    {
      *jobId = jobs[i]->getJobId();
      return jobs[i];
    }
  }
  return NULL;
}

void JobsList::printJobsList() {
  this->removeFinishedJobs();

  for (int i = 0; i < this->jobs.size(); i++) {
    cout << '[' << jobs[i]->getJobId() << ']' << " " << jobs[i]->getCmd() << " : " << jobs[i]->getJobPid() << ' ' << difftime(time(NULL), jobs[i]->getJobTime());
    if (jobs[i]->getIsStopped()) {
      cout << " (stopped)";
    }
    cout << endl;
  }
}

void JobsList::addJob(char* cmd, JobsList* jobList, pid_t jobPid, bool isStopped){
  this->removeFinishedJobs();
  jobs.push_back(new JobEntry(isStopped, cmd, jobList, jobPid));
}

void JobsList::addJob(JobsList::JobEntry* job){
  this->removeFinishedJobs();
  jobs.push_back(job);
}

void JobsList::killAllJobs() {
    cout << "smash: sending SIGKILL signal to " << jobs.size() << " jobs:" << endl;
    for (int i = 0; i < jobs.size(); i++) {
        cout << jobs[i]->getJobPid() << ": " << jobs[i]->getCmd() << endl;
        kill(jobs[i]->getJobPid(), SIGKILL);
    }
}

#endif //SMASH_COMMAND_H_
