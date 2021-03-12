#ifndef __HEADER__
#define __HEADER__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_MEM_SIZE 1048577
#define INPUT_LEN        100
#define MAX_CMD           11
#define MAX_OPR           11
#define MNEMONIC           7
#define FALSE              0
#define TRUE               1
#define NONE              -1
#define OPCODE             3
#define OPCODE_INPUT     30
#define OPTABLE_SIZE      20

char memory[MAX_MEM_SIZE];

#endif
