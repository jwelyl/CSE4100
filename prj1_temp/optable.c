#include "optable.h"

HashNode* head[OPTABLE_SIZE];
HashNode* tail[OPTABLE_SIZE];

HashNode* allocHN(char* opcode, char* mnemonic) {
  HashNode* node = (HashNode*)malloc(sizeof(HashNode));
  strcpy(node->opcode, opcode);
  strcpy(node->mnemonic, mnemonic);
  node->link = NULL;

  return node;
}

void push_node(char* opcode, char* mnemonic) {
  int idx = hash_function(mnemonic);
  HashNode* node = allocHN(opcode, mnemonic);

  if(!head[idx] && !tail[idx])  //  해당 인덱스에 처음 삽입하는 경우
    head[idx] = node;
  else // 이미 하나 이상의 노드가 삽입된 경우
    tail[idx]->link = node;
  tail[idx] = node;
}

int find_opcode(char* mnemonic, char* opcode) {
  int idx = hash_function(mnemonic);

  HashNode* cur = head[idx];

  for(; cur; cur = cur->link) {
    if(!strcmp(cur->mnemonic, mnemonic)) {
      strcpy(opcode, cur->opcode);
      return TRUE;
    }
  }
  
  return FALSE;
}

void print_optable() {
  int i = 0;
  for(i = 0; i < OPTABLE_SIZE; i++) {
    HashNode* cur = head[i];

    printf("%d : ", i);
    while(cur) {
      printf("[%s, %s]", cur->mnemonic, cur->opcode);
      if(cur->link) printf(" -> ");
      cur = cur->link;
    }
    printf("\n");
  }
}

void delete_optable() {
  int i = 0;
  for(i = 0; i < OPTABLE_SIZE; i++) {
    HashNode* del = head[i];

    while(del) {
      head[i] = del->link;
      free(del);
      del = head[i];
    }
  }
}

int mnemonic_sum(char* mnemonic) {
  int i = strlen(mnemonic);
  int num = 1;
  int ret = 0;

  for(; i >= 0; i--) {
    ret += ((mnemonic[i] - 'A') * num);
    num *= 26;
  }
   
  return ret;
}

int hash_function(char* mnemonic) {
  int len = strlen(mnemonic);
  int sum = mnemonic_sum(mnemonic);

  if(len == 1) return 1;
  else if(len == 6) return 11;
  else if(len == 3) {
    if(2041 <= sum && sum <= 58617) return 0;
    else if(93821 <= sum && sum <= 128739) return 1;
    else if(161239 <= sum && sum <= 166049) return 2;
    else if(195299 <= sum && sum <= 195429) return 3;
    else if(195583 <= sum && sum <= 195793) return 4;
    else if(195897 <= sum && sum <= 224653) return 5;
    else if(307203 <= sum && sum <= 328731) return 6;
    else if(329147 <= sum && sum <= 329277) return 7;
    else if(329355 <= sum && sum <= 329615) return 8;
    else if(329641 <= sum && sum <= 329849) return 9;
    else if(330551 <= sum && sum <= 339885) return 10;
  }
  else if(len == 4) {
    if(54821 <= sum && sum <= 55133) return 11;
    else if(1168453 <= sum && sum <= 1526109) return 12;
    else if(4442633 <= sum && sum <= 5842733) return 13;
    else if(5843045 <= sum && sum <= 8098441) return 14;
    else if(8560981 <= sum && sum <= 8577829) return 15;
    else if(8578141 <= sum && sum <= 8839077) return 16;
  }
  else if(len == 2 || len == 5) {
    if(9841 <= sum && sum <= 12857) return 17;
    else if(14885 <= sum && sum <= 30381533) return 18;
    else if(30381845 <= sum && sum <= 64680109) return 19;
  }
}

void make_optable() {
  FILE* fp = fopen("opcode.txt", "r");
  char input[OPCODE_INPUT];
  char opcode[OPCODE];
  char mnemonic[MNEMONIC];

  int i, start;
  int len, sum;
  
  opcode[OPCODE - 1] = '\0';
  mnemonic[MNEMONIC - 1] = '\0';

  while(TRUE) {
    if(!fgets(input, OPCODE_INPUT, fp)) break;

    for(i = 0; i < 2; i++)
      opcode[i] = input[i];
    for(i = 2; ; i++) {
      if(input[i] != ' ' && input[i] != '\t') {
        start = i;
        break;
      }
    }

    for(i = start; ; i++) {
      if(input[i] == ' ' || input[i] == '\t') {
        mnemonic[i - start] = '\0';
        break;
      }
      mnemonic[i - start] = input[i];
    }
    
    push_node(opcode, mnemonic);
  }

  fclose(fp);
}
