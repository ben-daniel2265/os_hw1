int MAX_ARGS = 20;
#include <iostream>
#include <string.h>
#include <unistd.h>
using namespace std;
const string WHITESPACE = " \n\r\t\f\v";

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

int main(int argc, char** argv) {
  cout << getcwd(NULL, 0) << endl;
  chdir("בולבול גדול ממש");
  cout << getcwd(NULL, 0) << endl;
  return 0;
}