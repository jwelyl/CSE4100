#ifndef __HEADER__
#define __HEADER__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_MEM_SIZE 1048577
#define CMD_LEN           11
#define MNEMONIC           7

char memory[MAX_MEM_SIZE];

//  operands
char mnemonic[MNEMONIC];
int dstart, dend;
int eaddress, evalue;
int fstart, fend, fvalue;

#endif
