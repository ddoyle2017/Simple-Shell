#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H
#pragma once

#include <iostream>

using namespace std;

int	monteCarloSim(char *command);
void 	sigint_handler(int sig);
void	sigtstp_handler(int sig);
void	shell_sigint_handler(int sig);
void	shell_sigtstp_handler(int sig);

#endif
