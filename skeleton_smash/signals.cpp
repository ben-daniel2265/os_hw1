#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"
#include <sys/wait.h>

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
  cout << "smash: got an alarm" << endl;

  SmallShell::TimedCommand* timedCmd = SmallShell::getInstance().popHeadTimedCommand();

  SmallShell::getInstance().getJobsList()->removeFinishedJobs();
  if(timedCmd->pid != -1) {
    if(waitpid(timedCmd->pid, NULL, WNOHANG) == 0){
      if(kill(timedCmd->pid, SIGKILL) == -1) {
        perror("smash error: kill failed");
      }
      else {
        cout << "smash: " << timedCmd->cmd->getTimedCommandLine() << " timed out!" << endl;
      }
    }
  }
  
  SmallShell::getInstance().resetAlarm();
}

