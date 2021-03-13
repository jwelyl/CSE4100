#ifndef __OPTABLE__
#define __OPTABLE__

#include "header.h"

typedef struct hashNode {
  char mnemonic[MNEMONIC];
  char opcode[OPCODE];
  struct hashNode* link;
} HashNode;

HashNode* allocHN(char* opcode, char* mnemonic);
void push_node(char* opcode, char* mnemonic);
int find_opcode(char* mnemonic, char* opcode);
void print_optable();
void delete_optable();
int mnemonic_sum(char* mnemonic);
int hash_function(char* mnemonic);
void make_optable();

#endif

