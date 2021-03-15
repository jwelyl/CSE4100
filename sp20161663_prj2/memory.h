#ifndef __MEMORY__
#define __MEMORY__

#include "20161663.h"

char memory[MAX_MEM_SIZE];

void reset_memory(/*char* memory*/);
void dump_memory(/*char* memory,*/ int start, int end);
void edit_value(/*char* memory,*/ int addr, int val);
void fill_value(/*char* memory,*/ int start, int end, int val);

#endif
