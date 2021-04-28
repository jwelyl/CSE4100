#include "symtable.h"
#include "memory.h"

SymTabNode* st_head[SYMTAB_SIZE]; //  assemble 과정에서 사용되는 hash table로 
                                  //  assemble 후 해제됨
SymTabNode* latest_symtable = NULL; //  가장 최근의 assemble 결과 label 저장

int st_num = 0;         //  symtable에 삽입된 label 개수
int latest_st_num = 0;  //  가장 최근에 assemble 후 생성된 symtable의 label 개수

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
    st_num++;
    return TRUE;
  }
  else {
    for(; cur; ) {
      if(strcasecmp(label, cur->label) < 0) { 
        if(cur == st_head[idx]) {//  삽입되는 위치가 가장 앞일 경우 
          node->next = cur;
          cur->prev = node;
          st_head[idx] = node;
        }
        else {                  //  중간에 삽입될 경우
          node->prev = cur->prev;
          cur->prev->next = node;
          node->next = cur;
          cur->prev = node;
        }

        st_num++;
        return TRUE;
      }

      if(cur->next == NULL) { //  삽입되는 위치가 가장 마지막일 경우
        cur->next = node;
        node->prev = cur;

        st_num++;
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
//    printf("유효하지 않은 label %s\n", label);
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
//    printf("유효하지 않은 label %s\n", label);
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
  
  //  형식에 맞게 가장 최근에 assemble 후 symbol, location counter 출력
  for(i = 0; i < latest_st_num; i++) {
    dec_to_hex(latest_symtable[i].locctr, locctr_col, LOCCTR_SIZE);
    printf("\t%s\t%s\n", latest_symtable[i].label, locctr_col);
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
  st_num = 0;
}

int st_hash_function(char* label) {
  //  label의 가장 앞의 알파벳으로 index 분류
  //  'A'부터 'Z'까지의 알파벳을 두 개씩 묶어 각각 0, 1, 2, ..., 12를 부여
        
  int idx;

  if('A' <= label[0] && label[0] <= 'Z')
    idx = (label[0] - 'A') / 2;
  else if('a' <= label[0] && label[0] <= 'z') 
    idx = (label[0] - 'a') / 2;
  else
    idx = NONE;

  return idx;
}

int make_latest_symtable() {
  //  assemble 성공 시 assemble 과정에서 symtable에 저장된 label, locctr 내용을
  //  동적할당한 배열에 저장한 후 symtable은 해제함
  //  매번 assemble 성공 시 호출되어 기존의 동적할당한 배열을 해제 후 새로 생성 
  int i, j = 0;
  
  if(st_num == 0) {
    printf("Label 개수 0개\n");
    return FALSE;
  }

  latest_st_num = st_num;
  st_num = 0;

  if(latest_symtable) { //  이전에 assemble이 성공하여 배열이 할당된 경우
    free(latest_symtable);  //  동적할당 해제
    latest_symtable = NULL;
  }

  latest_symtable = (SymTabNode*)malloc(sizeof(SymTabNode) * latest_st_num);

  for(i = 0; i < SYMTAB_SIZE; i++) {
    SymTabNode* cur = st_head[i];
    
    while(cur) {
        strcpy(latest_symtable[j].label, cur->label);
        latest_symtable[j].locctr = cur->locctr;
        j++;
        cur = cur->next;
    }
  }
  
  //  복사 후 symtable 해제
  delete_symtable();
  return TRUE;
}

void delete_latest_symtable() {
  //  프로그램이 종료될 때 호출되어 동적 할당된 배열을 해제한다.
  if(latest_symtable) {
    free(latest_symtable);
    latest_symtable = NULL;
  }
}
