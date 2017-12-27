#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fcntl.h>
#include <signal.h>
#include <limits.h>
#include <setjmp.h>

#include "signalHandler.h"


using namespace std;


void 	sigint_handler(int sig);
void 	sigtstp_handler(int sig);
void	sigsegv_handler(int sig);
void	sigalrm_handler(int sig);
void	shell_sigint_handler(int sig);
void	shell_sigtstp_handler(int sig);

void	signalUnitTests();
void	printResults();

static unsigned long int numEvals;
static unsigned long int numSegFaults;
jmp_buf bufferA;
jmp_buf bufferB;
jmp_buf bufferC;


int monteCarloSim(char *command) {
	
	numEvals = 0;
	numSegFaults = 0;
	srand(time(NULL));
	
	signal(SIGINT, sigint_handler);
	signal(SIGTSTP, sigtstp_handler);
	signal(SIGSEGV, sigsegv_handler);	
	
//	signalUnitTests();
	
	cout << endl << " ** Monte Carlo Simulation ** " << endl << endl;
	
	for (; numEvals < ULONG_MAX; numEvals++) 
	{
		sigsetjmp(bufferA, 1);
		
		if ((numEvals % 1000) == 0)
			cout << " . ";
			
		int  *source = (int*)rand();
		int   ptr    = *source;		
	}
	 
	printResults();

	return 0;
}


/**
 *
 * Helper Functions
 *
**/
void signalUnitTests() {
	if (raise(SIGINT) != 0)
		cout << "Unable to raise SIGINT" << endl;
	if (raise(SIGSEGV) != 0)
		cout << "Unable to raise SIGSEGV" << endl;
	if (raise(SIGTSTP) != 0)
		cout << "Unable to raise SIGTSTP" << endl;
}

void printResults() {
	double percent = (double)((numSegFaults/numEvals) * 100);
	cout << endl << "Total seg faults:  " << numSegFaults << endl;
	cout << "Total evals tried: " << numEvals << endl; 
	cout << "Percentage of segfaults: " << percent << endl << endl;		
}

/**
 *
 * Signal Handlers
 *
**/
void sigint_handler(int sig) {
	sigsetjmp(bufferB, 1);
	cout << endl << "CTRL-C: SIGINT handled" << endl;

	printResults();
	exit(EXIT_SUCCESS);
}

void sigtstp_handler(int sig) {
	sigsetjmp(bufferC, 1);
	cout << endl << "CTRL-Z: SGTSTP handled" << endl;

	printResults();
	siglongjmp(bufferA, 1);
}

void sigsegv_handler(int sig) {
	
	numSegFaults++;
	numEvals++;
	if (numEvals < ULONG_MAX) {
		siglongjmp(bufferA, 1);
	}
	else {
		printResults();
		exit(EXIT_SUCCESS);
	}
}

void sigalrm_handler(int sig) {
	
}

void shell_sigint_handler(int sig) {
	cout << endl << "CTRL-C was here" << endl;
	siglongjmp(bufferB, 1);	
}

void shell_sigtstp_handler(int sig) {	
	cout << endl << "CTRL-Z was here" << endl;
	siglongjmp(bufferC, 1);
}
