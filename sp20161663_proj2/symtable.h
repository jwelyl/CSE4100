#ifndef __SYMTABLE__
#define __SYMTABLE__

#include "20161663.h"

typedef struct symtabNode {
  char label[STRING_SIZE];
  int locctr;
  struct symtabNode* prev;
  struct symtabNode* next;
} SymTabNode; 

SymTabNode* allocSTN(char* label, int locctr);
int push_stnode(char* label, int locctr);
int find_label(char* label);
int find_locctr(char* label, int* locctr);
void print_symtable();
void delete_symtable();
int st_hash_function(char* label);

#endif
