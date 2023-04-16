define MAX_ARGS 20
char** breakCommandToParams(cosnt char* cmd_line)
{
  char** args = new char*[MAX_ARGS];
  int i = 0;
  while(cmd_s != "")
  {
    string  cmd_s = _trim(cmd_line);
    string  firstWord = cmd_s.substr(0, cmd_s.find_first_of(" "));
    args[i] = new char[firstWord.length()+1];
    strcpy(args[i], firstWord.c_str());
    cmd_s = cmd_s.substr(cmd_s.find_first_of(" ")+1);
    i++;
  }
  return args;

}

int main(int argc, char** argv) {
    char** test = breakCommandToParams(argv[1]);
    for(int i = 0; i < MAX_ARGS; i++) {
        if(test[i] != NULL) {
            cout << test[i] << endl;
        }
    }
}