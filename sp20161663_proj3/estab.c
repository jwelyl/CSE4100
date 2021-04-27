#include "estab.h"
#include "memory.h"

EstabNode* est_head[ESTAB_SIZE];  //  load 과정에서 사용되는 hash table로 load 후 해제됨
EstabNode load_map[REFERENCE_N];  //  load 후 load 맵 출력을 위한 동적 배열 

int est_num = 0;      //  estable에 삽입된 symbol 개수
int total_length = 0; //  program 총 길이

EstabNode* allocESTAB(char* sym_name, int sym_addr, int length) {
  EstabNode* node = (EstabNode*)malloc(sizeof(EstabNode));
  strcpy(node->symbol_name, sym_name);
  node->symbol_addr = sym_addr;
  node->length = length;
  node->is_sect = node->length == NONE ? FALSE : TRUE;
  //  legnth가 NONE일 경우 CSECT Name 아님.

  node->prev = NULL;
  node->next = NULL;

  return node;
}

//  hash table estab과 loadmap에 삽입하는 함수
int push_est_node(char* sym_name, int sym_addr, int length) {
  int idx = est_hash_function(sym_name);
  EstabNode* node = allocESTAB(sym_name, sym_addr, length);
  EstabNode* cur = est_head[idx];
  
  if(length != NONE)
    total_length += length;

  if(idx == NONE) {
    free(node);
    return FALSE;
  } 

  //  loadmap을 위한 1차원 배열에 삽입하는 과정
  strcpy(load_map[est_num].symbol_name, node->symbol_name);
  load_map[est_num].symbol_addr = node->symbol_addr;
  load_map[est_num].length = node->length;
  load_map[est_num].is_csect = node->is_csect; 

  //  hash table estable에 삽입하는 과정
  if(!cur) {  //  해당 인덱스에 처음 삽입하는 경우
    est_head[idx] = node;
    est_num++;
    return TRUE;
  } //  if-!cur end

  else {
    for(; cur; ) {
      if(strcasecmp(sym_name, cur->symbol_name) < 0) {
        if(cur == est_head[idx]) {  //  삽입되는 위치가 가장 앞일 경우
          node->next = cur;
          cur->prev = node;
          est_head[idx] = node;
        }  
        else {  //  중간에 삽입될 경우
          node->prev = cur->prev;
          cur->prev->next = node;
          node->next = cur;
          cur->prev = node;
        }

        est_num++;
        return TRUE;
      }
      
      if(cur->next == NULL) { //  삽입되는 위치가 가장 마지막일 경우
        cur->next = node;
        node->prev = cur;
        est_num++;

        return TRUE;
      }

      cur = cur->next;
    } //  for end 
  } // else end

  printf("estab 삽입 실패!\n");
  return FALSE;
} //  push end

//  symbol의 주소를 찾는 함수
int find_sym_addr(char* sym_name, int* addr) {
  int idx = est_hash_function(sym_name);
  if(idx == NONE) {
    printf("유효하지 않은 symbol %s\n", sym_name);
    return FALSE;
  }

  EstabNode* cur = est_head[idx];

  for(; cur; cur = cur->next) {
    if(!strcmp(cur->sym_name, sym_name)) {
      *addr = cur->symbol_addr;
      return TRUE;
    }
  }

  return FALSE;
}

int est_hash_function(char* sym_name) {
  //  symbol의 가장 앞의 알파벳으로 index 분류
  //  'A'부터 'Z'까지의 알파벳을 두 개씩 묶어 0, 1, 2, ... , 12를 부여

  int idx;

  if('A' <= sym_name[0] && sym_name[0] <= 'Z')
    idx = (sym_name[0] - 'A') / 2;
  else if('a' <= sym_name[0] && sym_name[0] <= 'z')
    idx = (sym_name[0] - 'a') / 2;
  else
    idx = NONE;

  return idx;
}

//  loadmap 출력
void print_loadmap() {
  int i;
  char hex[5];
  char output[INPUT_LEN] = {0, };

  printf("control symbol address length\n");
  printf("section name\n");
  printf("--------------------------------\n");
  
  for(i = 0; i < est_num; i++) {
    if(load_map[i].is_csect) {  //  control_section 이름일 경우
      strcpy(output, load_map[i].symbol_name);
      dec_to_hex(load_map[i].addr, hex, 5);
      strcat(output, "\t\t");
      strcat(output, hex);
      dec_to_hex(load_map[i].length, hex, 5);
      strcat(output, "\t");
      strcat(output, hex);
    }
    else {  //  control_section이 아닐경우
      strcpy(output, "\t");
      strcat(output, load_map[i].symbol_name);
      strcat(output, "\t");
      dec_to_hex(load_map[i].addr, hex, 5);
      strcat(output, hex);
    }
    fputs(output, stdout);
    puts();
  }

  printf("--------------------------------\n");

  dec_to_hex(total_length, hex, 5);
  printf("\t\ttotal length %s\n", hex); //  프로그램 길이 출력
}

//  load 과정이 끝난 후 hash table 해제
void delete_estable() {
  int i;
  for(i = 0; i < ESTAB_SIZE; i++) {
    EstabNode* del = est_head[i];

    while(del) {
      est_head[i] = del->next;
      free(del);
      del = est_head[i];
    }
    est_head[i];
  }
  est_num = 0;
}

