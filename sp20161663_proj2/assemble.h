#ifndef __ASSEMBLE__
#define __ASSEMBLE__

#include "20161663.h"

int pass_1(char* filename, char* mid_filename, FILE* fp_asm, FILE** fp_mid);
int pass_2(char* filename, char* mid_filename, char* lst_filename, char* obj_filename, 
          FILE* fp_asm, FILE** fp_mid, FILE** fp_lst, FILE** fp_obj);

#endif
