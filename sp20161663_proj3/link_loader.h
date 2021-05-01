#ifndef __LINK_LOADER_H__
#define __LINK_LOADER_H__

#include "20161663.h"

void add_bp(int bp, char* bpc);
void show_all_bps();
void clear_all_bps();
void set_progaddr(int prog_addr);
int loader_pass1(FILE* fp_obj1, FILE* fp_obj2, FILE* fp_obj3);
int loader_pass2(FILE* fp_obj1, FILE* fp_obj2, FILE* fp_obj3);

#endif

