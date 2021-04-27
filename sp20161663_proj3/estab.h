#ifndef __ESTAB_H__
#define __ESTAB_H__

#include "20161663.h"

typedef struct estab_node {
  char* symbol_name[SYMBOL_SIZE]; //  SYMBOL 이름(CONTROL_SECT 이름 또는 Define record 이름)
  int symbol_addr;    //  SYMBOL 주소(CONTROL_SECT 시작 주소 + Define record 주소)
  int length;         //  CONTROL_SECT 길이(CONTROL_SECT 이름이 아닐 경우 NONE)
  int is_csect;       //  CONTROL_SECT 이름일 경우 TRUE, 아닐 경우 FALSE
  struct estab_node* prev;  //  doubly linked list pointer
  struct estab_node* next;
} EstabNode;

EstabNode* allocESTAB(char* sym_name, int sym_addr, int length);
int push_est_node(char* sym_name, int sym_addr, int length);
int find_sym_addr(char* sym_name, int* addr);
int est_hash_function(char* sym_name);
void print_loadmap();
void delete_estable();

#endif
