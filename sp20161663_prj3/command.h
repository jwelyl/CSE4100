#ifndef __COMMNAD__
#define __COMMAND__

#include "20161663.h"

int invalid_command(char* input, char* cmd, int* opt_start);
int process_command(char* cmd, char* input, int opt_start);

#endif
