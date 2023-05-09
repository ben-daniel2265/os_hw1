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
#include <fcntl.h>
#include <sys/stat.h>


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

void _parseTimeout(const char* cmd_line, char* command, char* time) {
  FUNC_ENTRY()
  
  size_t timePlace = string(cmd_line).find(' ', string(cmd_line).find(time)+1);

  command = (char*)malloc(strlen(cmd_line)-timePlace+1);
  memset(command, 0, strlen(cmd_line)-timePlace+1);
  strcpy(command, cmd_line+timePlace+1);

  FUNC_EXIT()
}

void _splitPipeCommand(const char* cmd_line, char** cmd1, char** cmd2) {
  FUNC_ENTRY()
  size_t pipePlace = string(cmd_line).find('|');

  *cmd1 = (char*)malloc(pipePlace+1);
  *cmd2 = (char*)malloc(strlen(cmd_line)-pipePlace+1);

  memset(*cmd1, 0, pipePlace+1);
  memset(*cmd2, 0, strlen(cmd_line)-pipePlace+1);

  strncpy(*cmd1, cmd_line, pipePlace);
  strcpy(*cmd2, cmd_line+pipePlace+1);

  FUNC_EXIT()

}

void _splitRedirectionCommand(const char* cmd_line, char** cmd1, char** cmd2) {
  FUNC_ENTRY()
  size_t redirPlace = string(cmd_line).find('>');

  *cmd1 = (char*)malloc(redirPlace+1);
  *cmd2 = (char*)malloc(strlen(cmd_line)-redirPlace+1);

  memset(*cmd1, 0, redirPlace+1);
  memset(*cmd2, 0, strlen(cmd_line)-redirPlace+1);

  strncpy(*cmd1, cmd_line, redirPlace);
  strcpy(*cmd2, cmd_line+redirPlace+1);

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

int _isPipeCommand(const char* cmd_line) {
  const string str(cmd_line);
  if(str.find('|') != string::npos) {
    if(str.find("|&") != string::npos) return 2;
    return 1;
  }
  return 0;
}

int _isRedirectionCommand(const char* cmd_line) {
  const string str(cmd_line);
  if(str.find('>') != string::npos) {
    if(str.find(">>") != string::npos) return 2;
    return 1;
  }
  return 0;
}

bool isNumber(const char* str)
{
    for (int i = 0; i < strlen(str); i++)
    {
        if (!(isdigit(str[i]) || (str[i] == '-' && i == 0)))
        {
            return false;
        }
    }
    return true;
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


  if(_isPipeCommand(cmd_line)) {
    return new PipeCommand(cmd_line);
  }

  if(_isRedirectionCommand(cmd_line)) {
    return new RedirectionCommand(cmd_line);
  }

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
  else if(firstWord.compare("setcore") == 0) {
      return new SetcoreCommand(cmd_line);
  }
  else if(firstWord.compare("getfileinfo") == 0) {
      return new GetFileTypeCommand(cmd_line);
  }
  else if(firstWord.compare("chmod") == 0) {
      return new ChmodCommand(cmd_line);
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
      strcpy(argsBash[0] ,"bash");
      strcpy(argsBash[1],"-c");
      argsBash[2] = (char*)malloc(strlen(cmd_lineCopy)+1);
      argsBash[3] = NULL;
      strcpy(argsBash[2], cmd_lineCopy);

      if(execvp("bash", argsBash) == -1)
      {
        cerr << "smash error: > \"" << cmd_lineCopy << "\"" << endl;
        exit(0);
      }
    }
    else{
      if(execvp(args[0], args) == -1)
      {
          cerr << "smash error: > \"" << cmd_lineCopy << "\"" << endl;
          exit(0);
      }
    }
  }
  else if (pid > 0){
    if(this->getEndTime() != NULL){
      smash.addTimedJob(new SmallShell::TimedCommand(new ExternalCommand(this->getCmdLine(), this->getEndTime()), pid));
      smash.resetAlarm();
    }
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
  else {
    perror("smash error: > fork failed");
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
  SmallShell& smash = SmallShell::getInstance();
  cout << "smash pid is " << smash.getSmashPid() << endl;
}

void ChangeDirCommand::execute() {
  char* args[COMMAND_MAX_ARGS];
  char* tmepdir;

  SmallShell& smash = SmallShell::getInstance();
  int argCount = _parseCommandLine(this->getCmdLine(), args);

  if(argCount == 2) {
    if(strcmp(args[1], "-") == 0) {
      if(smash.getLastPwd() == NULL) {
        cerr<<"smash error: cd: OLDPWD not set\n";
      }
      else {
        tmepdir = getcwd(NULL, 0);
        chdir(smash.getLastPwd());
        smash.setLastPwd(tmepdir);
      }
    }
    else{
      char* temp = getcwd(NULL, 0);
      if(chdir(args[1]) == -1) {
        perror("smash error: chdir failed");
        return;
      }
      smash.setLastPwd(temp);
    }
  }
  else if(argCount >= 2){
    cerr << "smash error: cd: too many arguments" << endl;
  }
  else{
      string start = "smash error: ";
      char* temp = new char[strlen(start.c_str()) + strlen(this->getCmdLine()) + 1];
      strcpy(temp, start.c_str());
      strcat(temp, this->getCmdLine());
      cerr << temp << endl;
  }
}

void SetcoreCommand::execute() {
  SmallShell& smash = SmallShell::getInstance();

  char* args[COMMAND_MAX_ARGS];
  int argCount = _parseCommandLine(this->getCmdLine(), args);

  if(argCount != 3 || !isNumber(args[1]) || !isNumber(args[2])) {
    cerr << "smash error: setcore : invalid arguments" << endl;
    return;
  }
  
  int jobID = atoi(args[1]);
  int limit = atoi(args[2]);

  JobsList::JobEntry* job = smash.getJobsList()->getJobById(jobID);
  if(job == nullptr) {
    cerr << "smash error: setcore : job-id " << jobID << " does not exist" << endl;
    return;
  }

  int maxCore = sysconf(_SC_NPROCESSORS_ONLN);
  if (limit >= maxCore) {
    cerr << "smash error: setcore : invalid core number" << endl;
    return;
  }

  cpu_set_t set;
  CPU_ZERO(&set);
  CPU_SET(limit, &set);
  sched_setaffinity(job->getJobPid(), sizeof(cpu_set_t), &set);
}

void GetFileTypeCommand::execute() {
  SmallShell& smash = SmallShell::getInstance();

  char* args[COMMAND_MAX_ARGS];
  int argCount = _parseCommandLine(this->getCmdLine(), args);

  if(argCount != 2) {
    cerr << "smash error: file : invalid arguments" << endl;
    return;
  }

  struct stat statbuf;

  if (stat(args[1], &statbuf) == -1) {
    cerr << "smash error: file : invalid arguments" << endl;
    return;
  }

  char* type;

  switch (statbuf.st_mode & S_IFMT) {
    case S_IFBLK:  type = (char*)"block device";            break;
    case S_IFCHR:  type = (char*)"character device";        break;
    case S_IFDIR:  type = (char*)"directory";               break;
    case S_IFIFO:  type = (char*)"FIFO";                    break;
    case S_IFLNK:  type = (char*)"symbolic link";           break;
    case S_IFREG:  type = (char*)"regular file";            break;
    case S_IFSOCK: type = (char*)"socket";                  break;
    default:       type = (char*)"unknown?";                break;
  }
  

  cout << args[1] << "'s type is " << type << " and takes up " << statbuf.st_size << " bytes" << endl;

}

void ChmodCommand::execute() {
  SmallShell& smash = SmallShell::getInstance();

  char* args[COMMAND_MAX_ARGS];
  int argCount = _parseCommandLine(this->getCmdLine(), args);

  if(argCount != 3 || !isNumber(args[1])) {
    cerr << "smash error: chmod : invalid arguments" << endl;
    return;
  }

  int mode = atoi(args[1]);
  char* path = args[2];

  if (chmod(path, mode) == -1) {
    cerr << "smash error: chmod : invalid arguments" << endl;
    return;
  }
}

void JobsCommand::execute() {
  SmallShell& smash = SmallShell::getInstance();
  smash.getJobsList()->printJobsList();
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
            cerr << "smash error: fg: jobs list is empty" << endl;
        }
        else {
            if (job->getIsStopped()) {
                kill(job->getJobPid(), SIGCONT);
                job->setIsStopped(false);
            }
            smash.setFgJob(job);
            jobs->removeJobById(LastJobId);
            cout << job->getCmd() << " : " << job->getJobPid() << endl;
            waitpid(job->getJobPid(), NULL, WUNTRACED);
            smash.setFgJob(nullptr);

        }
    }
    else if(argCount > 2 || !isNumber(args[1]))
    {
        cerr << "smash error: fg: invalid arguments" << endl;
    }
    else
    {
        JobsList::JobEntry* job = jobs->getJobById(atoi(args[1]));
        if(job == nullptr)
        {
            cerr << "smash error: fg: job-id " << args[1] << " does not exist" <<endl;
        }
        else {
            if (job->getIsStopped()) {
                kill(job->getJobPid(), SIGCONT);
                job->setIsStopped(false);
            }
            smash.setFgJob(job);
            jobs->removeJobById(atoi(args[1]));
            cout << job->getCmd() << " : " << job->getJobPid() << endl;
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
            cerr << "smash error: bg: there is no stopped jobs to resume" << endl;
        }
        else
        {
            kill(job->getJobPid(), SIGCONT);
            job->setIsStopped(false);
            cout << job->getCmd() << " : " << job->getJobPid() << endl;
        }
    }
    else if (argCount > 2 || !isNumber(args[1])) {
        cerr << "smash error: bg: invalid arguments" << endl;
    }
    else
    {
        job = jobs->getJobById(atoi(args[1]));
        if(job == nullptr)
        {
            cerr << "smash error: bg: job-id " << args[1] << " does not exist" << endl;
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
                cerr << "smash error: bg: job-id " << args[1] << " is already running in the background" << endl;
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
    

    if(argCount < 3) {
      cerr << "smash error: kill: invalid arguments" << endl;
      return;
    }
    
    if(jobs->getJobById(atoi(args[2])) == NULL) {
      cerr << "smash error: kill: job-id " << args[2] << " does not exist" << endl;
      return;
    }

    JobsList::JobEntry* job = jobs->getJobById(atoi(args[2]));
    string sigNum = ((string)args[1]).substr(1);

    if(argCount != 3 || args[1][0] != '-' || !isNumber(sigNum.c_str()) || !isNumber(args[2]) || atoi(sigNum.c_str()) < 0 || atoi(sigNum.c_str()) > 31)
    {
       cerr << "smash error: kill: invalid arguments" << endl;
    }
    else if(job != nullptr)
    {
        if(kill(job->getJobPid(), atoi(sigNum.c_str())) != 0) {
            perror("smash error: kill failed");
        }
        else {
            if(atoi(sigNum.c_str()) == SIGSTOP)
            {
                job->setIsStopped(true);
            }

        }
        cout << "signal number " << sigNum << " was sent to pid " << job->getJobPid() << endl;
    }
    else
    {
        cerr << "smash error: kill: job-id " << args[2] << " does not exist" << endl;
    }

}

void PipeCommand::execute() {
    SmallShell& smash = SmallShell::getInstance();
    char* cmd1;
    char* cmd2;

    int pipeType = _isPipeCommand(this->getCmdLine());
    _splitPipeCommand(this->getCmdLine(), &cmd1, &cmd2);
    char* args1[COMMAND_MAX_ARGS];
    int argCount1 = _parseCommandLine(cmd1, args1);
    char* args2[COMMAND_MAX_ARGS];

    int argCount;
    if(pipeType == 2){
      argCount = _parseCommandLine(cmd2+1, args2);
    }
    else{
      argCount = _parseCommandLine(cmd2, args2);
    }

    int pipefd[2];
    if(pipe(pipefd) == -1)
    {
        perror("smash error: pipe failed");
        return;
    }

    pid_t pid1 = fork();

    if(pid1 == 0) {
      setpgrp();
      dup2(pipefd[1], pipeType);
      close(pipefd[0]);
      close(pipefd[1]);
      
      smash.CreateCommand(cmd1)->execute();
      exit(0);
    }
    else if(pid1 < 0){
        perror("smash error: fork failed");
        return;
    }

    pid_t pid2 = fork();

    if(pid2 == 0) {
      setpgrp();
      dup2(pipefd[0], 0);
      close(pipefd[0]);
      close(pipefd[1]);
      
      if(pipeType == 2){
        smash.CreateCommand(cmd2+1)->execute();
      }
      else{
        smash.CreateCommand(cmd2)->execute();
      }

      exit(0);
    }
    else if(pid2 < 0){
        perror("smash error: fork failed");
        return;
    }

    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid1, NULL, WUNTRACED);
    waitpid(pid2, NULL, WUNTRACED);
}

void RedirectionCommand::execute() {
    SmallShell& smash = SmallShell::getInstance();
    char* cmd;
    char* file;

    int redType = _isRedirectionCommand(this->getCmdLine());
    _splitRedirectionCommand(this->getCmdLine(), &cmd, &file);

    int fd;
    if(redType == 2)
    {
      strcpy(file, _trim(string(file+1)).c_str());
      fd = open(file, O_CREAT | O_WRONLY | O_APPEND, 0666);
    }
    else{
      strcpy(file, _trim(string(file)).c_str());
      fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, 0666);
    }

    if(fd == -1)
    {
      perror("smash error: open failed");
      return;
    }

    pid_t pid = fork();
    if(pid == 0) {
      setpgrp();
      dup2(fd, 1);
      close(fd);

      smash.CreateCommand(cmd)->execute();
      exit(0);
    }
    else if(pid < 0){
        perror("smash error: fork failed");
        return;
    }

    close(fd);
    waitpid(pid, NULL, WUNTRACED);
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
    cout << '[' << jobs[i]->getJobId() << ']' 
         << " " << jobs[i]->getCmd() << " : " 
         << jobs[i]->getJobPid() << ' ' 
         << difftime(time(NULL), jobs[i]->getJobTime()) << " secs";

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
  // insersts the job in the right place
  for (int i = 0; i < jobs.size(); i++)
  {
    if (jobs[i]->getJobId() > job->getJobId())
    {
      jobs.insert(jobs.begin() + i, job);
      return;
    }
  }
  jobs.push_back(job);
}

void JobsList::killAllJobs() {
    cout << "smash: sending SIGKILL signal to " << jobs.size() << " jobs:" << endl;
    for (int i = 0; i < jobs.size(); i++) {
        cout << jobs[i]->getJobPid() << ": " << jobs[i]->getCmd() << endl;
        kill(jobs[i]->getJobPid(), SIGKILL);
    }
}


void TimeoutCommand::execute() {
    SmallShell& smash = SmallShell::getInstance();
    
    char* args[COMMAND_MAX_ARGS];
    int argCount = _parseCommandLine(this->getCmdLine(), args);

    if (argCount < 3)
    {
      cout << "smash error: timeout: invalid arguments" << endl;
      return;
    }

    char* command;
    _parseTimeout(this->getCmdLine(), &command, args[1]);

    int duration = atoi(args[1]);

    if(duration <= 0)
    {
      cout << "smash error: timeout: invalid arguments" << endl;
      return;
    }

    time_t endTime = time(NULL) + duration;

    Command* cmd = smash.CreateCommand(command);

    cmd->setEndTime(endTime);

    if(!cmd->isExternal()) {
      smash.addTimedJob(new SmallShell::TimedCommand(cmd, -1));
      smash.resetAlarm();
    }

    cmd->execute();
}

#endif //SMASH_COMMAND_H_

// -------------------------------