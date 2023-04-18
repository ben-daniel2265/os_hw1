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
const char* DEFAULT_PROMPT = "smash";

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

  Command* cmd = CreateCommand(cmd_line);
  if(cmd->isExternal()) {
  }
  else{
    cmd->execute();
  }
}

void ChpromptCommand::execute() {
  SmallShell& smash = SmallShell::getInstance();

  char* args[COMMAND_MAX_ARGS];
  int argCount = _parseCommandLine(this->getCmdLine(), args);

  if(argCount == 1) {
    smash.setPrompt(DEFAULT_PROMPT);
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
    perror("smash error:> \“command-line\”\n");
  }
}

void JobsCommand::execute() {
  this->jobs->printJobsList();
}

void JobsList::printJobsList() {
  for (int i = 0; i < this->jobs.size(); i++) {
    cout << '[' << jobs[i]->getJobId() << ']' << ' : ' << jobs[i]->getCmd() << ' ' << jobs[i]->getJobPid() << ' ' << difftime(time(NULL), jobs[i]->getJobTime());
    if (jobs[i]->getIsStopped()) {
      cout << " (stopped)";
    }
    cout << endl;
  }
}
