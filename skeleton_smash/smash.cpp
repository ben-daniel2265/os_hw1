#ifndef SMASH_CPP
#define SMASH_CPP

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "Commands.h"
#include "signals.h"

int main(int argc, char* argv[]) {
    if(signal(SIGTSTP , ctrlZHandler)==SIG_ERR) {
        cerr << "smash error: failed to set ctrl-Z handler" << endl;
    }
    if(signal(SIGINT , ctrlCHandler)==SIG_ERR) {
        cerr << "smash error: failed to set ctrl-C handler" << endl;
    }

    struct sigaction* alarmHandlerAc = new struct sigaction;
    alarmHandlerAc->sa_handler = alarmHandler;
    alarmHandlerAc->sa_flags = SA_RESTART;
    if(sigaction (SIGALRM, alarmHandlerAc, NULL) == -1) {
        cerr << "smash error: failed to set alarm handler" << endl;
    }

    //TODO: setup sig alarm handler

    SmallShell& smash = SmallShell::getInstance();
    while(true) {
        std::cout << smash.getPrompt() << "> ";
        std::string cmd_line;
        std::getline(std::cin, cmd_line);
        if(cmd_line == "") continue;
        smash.executeCommand(cmd_line.c_str());
    }
    return 0;
}

#endif //SMASH_CPP