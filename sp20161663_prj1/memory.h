#ifndef __MEMORY__
#define __MEMORY__

#include "20161663.h"

char memory[MAX_MEM_SIZE];
char address_col[ADDR_LEN];
char print_col[PRINT_LEN];
char ascii_col[PRINT_LEN];

void reset_memory();
void dump_memory(int start, int end);
void edit_value(int addr, int val);
void fill_value(int start, int end, int val);

#endif
