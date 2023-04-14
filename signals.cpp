#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"
#include <sys/wait.h>


using namespace std;

void ctrlZHandler(int sig_num) {
	// TODO: Add your implementation
}

void ctrlCHandler(int sig_num) {
  // TODO: Add your implementation
}

void alarmHandler(int sig_num) {
  // TODO: Add your implementation
}

void sigChildHandler(int sig_num){
    cout << "did wait for child! The zombie is dead.." << endl;
    wait(nullptr);
}

