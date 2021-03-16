#ifndef __MEMORY__
#define __MEMORY__

#include "20161663.h"

char memory[MAX_MEM_SIZE];

void reset_memory();
void dump_memory(int start, int end);
void edit_memory(int addr, int val);
void fill_memory(int start, int end, int val);

#endif
