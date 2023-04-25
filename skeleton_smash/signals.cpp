#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"

using namespace std;

void ctrlZHandler(int sig_num) {
	// TODO: Add your implementation
  cout << "smash: got ctrl-Z" << endl;
  SmallShell& smash = SmallShell::getInstance();
  if (smash.getFgJob() == nullptr) {
    return;
  }
  
  kill(smash.getFgJob()->getJobPid(), SIGTSTP);
  smash.getFgJob()->setIsStopped(true);
  smash.getFgJob()->setJobTime(time(NULL));
  smash.getJobsList()->addJob(smash.getFgJob());

  cout << "smash: process " << smash.getFgJob()->getJobPid() << " was stopped" << endl;
  smash.setFgJob(nullptr);
}

void ctrlCHandler(int sig_num) {
    cout << "smash: got ctrl-C" << endl;
    SmallShell& smash = SmallShell::getInstance();
    if (smash.getFgJob() == nullptr) {
        return;
    }
    kill(smash.getFgJob()->getJobPid(), SIGINT);
}

void alarmHandler(int sig_num) {
  // TODO: Add your implementation
}

