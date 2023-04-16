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

char ** breakCommandToParams(char* cmd_line, int* argsCount) {
  char** args = new char*[MAX_ARGS];
  for (int i = 0; i < MAX_ARGS; i++) {
    args[i] = NULL;
  }

  int i = 0;
  string cmd_s = _trim(cmd_line);
  while(cmd_s != "")
  {
    cmd_s = _trim(cmd_s);
  
    if (cmd_s.find_first_of(WHITESPACE) == string::npos)
    {
      args[i] = new char[cmd_s.length()+1];
      strcpy(args[i], cmd_s.c_str());
      i++;
      (*argsCount)++;
      break;
    }

    string firstWord = cmd_s.substr(0, cmd_s.find_first_of(WHITESPACE));
    args[i] = new char[firstWord.length()+1];
    strcpy(args[i], firstWord.c_str());
    cmd_s = cmd_s.substr(cmd_s.find_first_of(WHITESPACE)+1, string::npos);
    i++;
    (*argsCount)++;
  }
  return args;
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

SmallShell::SmallShell() {
// TODO: add your implementation
}

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
    continue;
  }
  else{
    cmd->execute();
  }
}


void ChpromptCommand::execute() {
  SmallShell smash = SmallShell.getInstance();

  char** args;
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
  char** args;
  char* tmepdir;
  SmallShell smash = SmallShell.getInstance();
  int argCount = _parseCommandLine(this->getCmdLine(), args);

  if(argCount == 2) {
    if(args[1] == "-") {
      if(smash.lastPwd == NULL) {
        cerr << "smash error: cd: OLDPWD not set" << endl;
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
    cerr << "smash error: cd: too many arguments" << endl;
  }
  else{
    cerr << "smash error:> \“command-line\”" << endl;
  }
}


