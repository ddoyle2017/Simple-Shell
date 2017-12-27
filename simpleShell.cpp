#include <iostream>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <dirent.h>
#include <signal.h>
#include <fcntl.h>

#include "signalHandler.h"


using namespace std;


#define CD_SUCCESS  10
#define CD_FAILURE -10
#define SUCCESS	    20
#define FAILURE    -20
#define PATH_MAX    4096

//
// Function Definitions
//
bool	isNumber(char* command);
void 	getDeadChildUsage();
int 	shellChangeDirectory(char **argList);
string 	shellCurrentDirectory();
int 	shellListFiles(char **argList);
void 	shellHelp();

void 	clearArgList(char **argList);
void 	createProcess(char **argList);
int 	executeUserCommand(char **argList);
void 	parseUserCommand(char *userCommand, char **argList);
void 	shellIntro();	

//
//
// main:
//
//
int main(int argc, char **argv) {
	bool   	loop = true;
	string 	currentDir = string("");
	int 	filePathTrim = shellCurrentDirectory().length(); // used to shorten filepath on the cmd line	

	shellIntro();
			
	while(loop) {
		char  userCommand[100];	// store the command entered by the user
		char *argList[50];	// store the tokenized arguments from the command
		int   commandStatus;    // store the results of the last executed command
				

		cout << "simpleShell ~" << currentDir  << " $ ";
		cin.getline(userCommand, 100);


		parseUserCommand(userCommand, argList);			
	        commandStatus = executeUserCommand(argList);
		
		if (commandStatus == CD_SUCCESS) {
			currentDir = shellCurrentDirectory();

			if (currentDir.length() >= filePathTrim) {
				currentDir.erase(currentDir.begin(), currentDir.begin()+filePathTrim);
			}		
		}
	    	
		clearArgList(argList);		
	}
	
	return 0;
}


//
//
// Functions:
//
//
void clearArgList(char **argList) {
	char empty[] = " ";
	
	for (int i = 0; i < 50; i++) { 
		argList[i] = empty;		
	}
}


void createProcess(char **argList) {
	pid_t pid;
	pid_t wpid;
	int status;
	
	pid = fork();	// create a new process

	if (pid < 0){	// fork failed, exit
		perror("error");
		exit(EXIT_FAILURE);
	}
	else if (pid == 0) {
		if (strcmp(argList[0], "sim") == 0) {
			monteCarloSim(argList[0]);
		}
		else if (execvp(argList[0], argList) == -1) {
			perror("error");
		} 
	}
	else { // parent process goes here
		do {
			if (wpid = waitpid(pid, &status, WUNTRACED) == -1) {
				perror("error");
				exit(EXIT_FAILURE);
			}
			/*
			if (strcmp(argList[0], "sim") == 0) {
				signal(SIGINT, shell_sigint_handler);
				signal(SIGTSTP, shell_sigtstp_handler);
			}
			*/
			cout << "  (Child PID: " << pid << ")"  << endl;
			getDeadChildUsage();

			if (WIFEXITED(status)) {
				cout << "  (exit status: " << WEXITSTATUS(status) << ")" << endl;
			}
			else if (WIFSIGNALED(status)) {
				cout << "  (interupted by signal: " << WTERMSIG(status) << ")" << endl;
			}
			
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));	
	}
}


int executeUserCommand(char **argList) 
{
	// check if command is a builtin
	if (strcmp(argList[0], "exit") == 0) 
	{	
		exit (EXIT_SUCCESS);		
	}
	else if (strcmp(argList[0], "cd") == 0) 
	{
		int status = shellChangeDirectory(argList);

		if (status == 0) 
		{			
			return CD_SUCCESS;
		}
		else 
		{
			perror("error");
			return CD_FAILURE;
		}
	} /*
	else if (strcmp(argList[0], "ls") == 0) 
	{
		return shellListFiles(argList);
	}
	*/
	else if (strcmp(argList[0], "pwd") == 0)
	{
		cout << shellCurrentDirectory() << endl;
		return SUCCESS;
	}
	else if (strcmp(argList[0], "help") == 0) 
	{
		shellHelp();
		return SUCCESS;		
	}
	else {
		createProcess(argList);
	}

	return 0;
}


void parseUserCommand(char *userCommand, char **argList) {
	int   argCount = 0;	// counts the number of arguments
	char *ptr;		// points to an individual argument

	ptr = strtok(userCommand, " ");
	while (ptr != NULL) {
		argList[argCount++] = ptr;
		ptr = strtok(NULL, " ");
	}
	argList[argCount] = NULL;
}


void shellIntro() {
	cout << endl;
	cout << "Welcome to simpleShell!" << endl;
	cout << "  * Daniel Doyle" << endl;
	cout << "  * ddoyle4" << endl;
	cout << endl;
}


//
//
// Helper Functions:
//
//
bool isNumber(char *command) {
	for (int i = 0;	i < strlen(command); i++) {
		if (!isdigit(command[i]))
			return false;
	}
	return true;
}

void getDeadChildUsage() {
	struct rusage processUsage;
	
	if (getrusage(RUSAGE_CHILDREN, &processUsage) == 0) {
		cout << "  (page faults: " << processUsage.ru_majflt << " | ";
		cout << "signals received: "  << processUsage.ru_nsignals << ")"  << endl;
	}
	else {
		perror("error");
	}
}


int shellChangeDirectory(char **argList) {
	
	if (argList[1] == NULL) {
		return 1;
	}
	else if (strcmp(argList[1], "--") == 0) {
		return chdir("..");
	}
	else { 
		return chdir(argList[1]);
	}
}


string shellCurrentDirectory() {
	char filePath[PATH_MAX];

	if ( getcwd(filePath, PATH_MAX) != NULL ) {
		return string(filePath);
	}
	else {
		return string("");
	}
}


int shellListFiles(char **argList) {
	DIR 		*directory;		// pointer to the actual directory
	struct dirent 	*entries; 		// struct containing all directory entries
	const char 	*filePath;		// file path to the directory

	
	// check and see if you can open the directory, return if you can't
	filePath = shellCurrentDirectory().c_str();	

	if ((directory = opendir(filePath)) == NULL) {
		cerr << "error: could not open directory" << endl;
		return FAILURE;
	}
	
	// read through all the entries and print out each one
	while ((entries = readdir(directory)) != NULL) {
		if (strcmp(argList[1], "-a") == 0)	
			cout << entries->d_name << "  ";
		else {
			if (entries->d_name[0] != '.')
				cout << entries->d_name << "  ";
		}	
	}
	cout << endl;
	closedir(directory);

	return SUCCESS;	
}


void shellHelp() {
	cout << "Built-In Commands:" << endl;
	cout << "  * cd    - change directories" << endl;
	cout << "  * ls    - list files in directory" << endl;
	cout << "  * pwd   - get current directory file path" << endl;
	cout << "  * help  - list shell built-in commands" << endl;
	cout << "  * sim   - run the Monte Carlo simulation" << endl;
	cout << "  * exit  - exit shell" << endl;
}

