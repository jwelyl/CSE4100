#include "symtable.h"
#include "memory.h"

SymTabNode* st_head[SYMTAB_SIZE];
SymTabNode* lastest_symtable[INPUT_LEN];

SymTabNode* allocSTN(char* label, int locctr) {
  SymTabNode* node = (SymTabNode*)malloc(sizeof(SymTabNode));
  strcpy(node->label, label);
  node->locctr = locctr;
  node->prev = NULL;
  node->next = NULL;

  return node;
}

int push_stnode(char* label, int locctr) {
  int idx = st_hash_function(label);
  SymTabNode* node = allocSTN(label, locctr);
  SymTabNode* cur = st_head[idx];

  if(idx == NONE) {
    free(node);
    return FALSE;
  }

  if(!cur) {//  해당 인덱스에 처음 삽입하는 경우
    st_head[idx] = node;
    return TRUE;
  }
  else {
    for(; cur; ) {
      if(strcasecmp(label, cur->label) < 0) { 
        if(cur == st_head[idx]) //  삽입되는 위치가 가장 앞일 경우 
          st_head[idx] = cur;
        else {                  //  중간에 삽입될 경우
          node->prev = cur->prev;
          cur->prev->next = node;
        }
        node->next = cur;
        cur->prev = node;
        return TRUE;
      }

      if(cur->next == NULL) { //  삽입되는 위치가 가장 마지막일 경우
        cur->next = node;
        node->prev = cur;
        return TRUE;
      }

      cur = cur->next;
    }
  }

  printf("symtab 삽입 실패!\n");
  return FALSE;
}

int find_label(char* label) {
  int idx = st_hash_function(label);
  if(idx == NONE) {
    printf("유효하지 않은 label\n");
    return FALSE;
  }  

  SymTabNode* cur = st_head[idx];

  for(; cur; cur = cur->next) {
    if(!strcmp(cur->label, label)) 
      return TRUE;
  }

  return FALSE;
}

int find_locctr(char* label, int* locctr) { //  label의 LOCCTR 찾기
  int idx = st_hash_function(label);
  if(idx == NONE) {
    printf("유효하지 않은 label\n");
    return FALSE;
  }  

  SymTabNode* cur = st_head[idx];

  for(; cur; cur = cur->next) {
    if(!strcmp(cur->label, label)) {
      *locctr = cur->locctr;
      return TRUE;
    }
  }

  return FALSE;
}

void print_symtable() {
  char locctr_col[LOCCTR_SIZE];
  int i;

  printf("symbol\n");
  for(i = 0; i < SYMTAB_SIZE; i++) {
    SymTabNode* cur = st_head[i];
    
    while(cur) {
      dec_to_hex(cur->locctr, locctr_col, LOCCTR_SIZE);
      printf("\t%s\t%s\n", cur->label, locctr_col);
    }
  }
}

void delete_symtable() {
  int i;
  for(i = 0; i < SYMTAB_SIZE; i++) {
    SymTabNode* del = st_head[i];

    while(del) {
      st_head[i] = del->next;
      free(del);
      del = st_head[i];
    }
    st_head[i] = NULL;
  }
}

int st_hash_function(char* label) {
  //  label의 가장 앞의 알파벳으로 index 분류
  //  'A'부터 'Z'까지의 알파벳을 두 개씩 묶어 각각 0, 1, 2, ..., 12를 부여
        
  int idx;

  if('A' <= label[0] && label[0] <= 'z')
    idx = (label[0] - 'A') / 2;
  else if('a' <= label[0] && label[0] <= 'z') 
    idx = (label[0] - 'a') / 2;
  else
    idx = NONE;

  return idx;
}
