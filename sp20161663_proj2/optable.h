#ifndef __OPTABLE__
#define __OPTABLE__

#include "20161663.h"

//  format 추가
typedef struct hashNode {
  char mnemonic[MNEMONIC];
  char opcode[OPCODE];
  char format[FORMAT];
  struct hashNode* link;
} HashNode;

//  format 매개변수 추가
HashNode* allocHN(char* opcode, char* mnemonic, char* format);
void push_node(char* opcode, char* mnemonic, char* format);
int find_opcode(char* mnemonic, char* opcode);
//  format 찾기 위한 함수 추가
int find_format(char* mnemonic);

void print_optable();
void delete_optable();
int mnemonic_sum(char* mnemonic);
int hash_function(char* mnemonic);
void make_optable();

#endif

