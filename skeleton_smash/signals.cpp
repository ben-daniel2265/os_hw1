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
  
  kill(smash.getFgJob()->getJobPid(), SIGSTOP);
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
    if(kill(smash.getFgJob()->getJobPid(), SIGKILL) == -1) {
        perror("smash error: kill failed");
    }
    else {
        smash.getJobsList()->removeJobById(smash.getFgJob()->getJobId());
        cout << "smash: process " << smash.getFgJob()->getJobPid() << " was killed" << endl;
        smash.setFgJob(nullptr);
    }

}

void alarmHandler(int sig_num) {
  // TODO: Add your implementation
}

