#include "optable.h"

HashNode* head[OPTABLE_SIZE]; //  linked-list의 가장 앞의 원소를 가리키는 포인터 저장 배열
HashNode* tail[OPTABLE_SIZE]; //  linked-list의 가장 뒤의 원소를 가리키는 포인터 저장 배열
HashNode* allocHN(char* opcode, char* mnemonic, char* format) { //  hash table에 삽입할 노드를 할당
  HashNode* node = (HashNode*)malloc(sizeof(HashNode)); 
  strcpy(node->opcode, opcode); //  노드에 opcode 복사
  strcpy(node->mnemonic, mnemonic); //  노드에 mnemonic 복사
  strcpy(node->format, format); //  노드에 format 복사
  node->link = NULL;

  return node;
}

void push_node(char* opcode, char* mnemonic, char* format) {  //  hash table에 노드 삽입
  int idx = hash_function(mnemonic);  //  hash table의 index 찾기
  HashNode* node = allocHN(opcode, mnemonic, format);

  if(idx == NONE) { //  삽입할 index 없음
    free(node); //  할당한 노드 해제
    return; 
  }

  if(!head[idx] && !tail[idx])  //  해당 인덱스에 처음 삽입하는 경우
    head[idx] = node;
  else // 이미 하나 이상의 노드가 삽입된 경우
    tail[idx]->link = node;
  tail[idx] = node;
}

int find_opcode(char* mnemonic, char* opcode) { //  해당 mnemonic을 찾을 경우 TRUE, 존재하지 않을 경우 FALSE 반환
  int idx = hash_function(mnemonic);
  if(idx == NONE) return FALSE; //  탐색 실패(찾을 위치 선정 불가)

  HashNode* cur = head[idx];

  for(; cur; cur = cur->link) {
    if(!strcmp(cur->mnemonic, mnemonic)) {
      strcpy(opcode, cur->opcode);
      return TRUE;
    }
  }
  
  return FALSE;
}

int find_format(char* mnemonic) {
  int idx = hash_function(mnemonic);
  if(idx == NONE) return NONE;  //  탐색 실패

  HashNode* cur = head[idx];

  for(; cur; cur = cur->link) {
    if(!strcmp(cur->mnemonic, mnemonic)) {
      if(!strcmp(cur->format, "1")) //  1형식
        return 1;
      else if(!strcmp(cur->format, "2")) // 2형식
        return 2;
      else if(!strcmp(cur->format, "3/4"))  //  3/4형식
        return 3;
    }
  }

  return NONE;
}

void print_optable() {  //  형식에 맞게 optable을 출력
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

void delete_optable() { //  프로그램이 종료되기 전, 동적할당된 노드를 해제
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

int mnemonic_sum(char* mnemonic) {  //  mnemonic을 26진법으로 계산하여 10진법으로 환산 후 반환
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
  //  1. 길이별로 저장될 index 분류
  //  2. 같은 길이일 경우 10진법으로 바꾼 mnemonic 값 기준으로 index 분류 
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
    else return NONE;
  }
  else if(len == 4) {
    if(54821 <= sum && sum <= 55133) return 11;
    else if(1168453 <= sum && sum <= 1526109) return 12;
    else if(4442633 <= sum && sum <= 5842733) return 13;
    else if(5843045 <= sum && sum <= 8098441) return 14;
    else if(8560981 <= sum && sum <= 8577829) return 15;
    else if(8578141 <= sum && sum <= 8839077) return 16;
    else return NONE;
  }
  else if(len == 2 || len == 5) {
    if(9841 <= sum && sum <= 12857) return 17;
    else if(14885 <= sum && sum <= 30381533) return 18;
    else if(30381845 <= sum && sum <= 64680109) return 19;
    else return NONE;
  }
  else return NONE;
}

void make_optable() { //  opcode.txt 파일을 읽어서 hash-table인 optable 생성
  FILE* fp = fopen("opcode.txt", "r");
  if(!fp) {
    printf("'opcode.txt' file does not exist.\n");
    exit(-1);
  }

  char input[OPCODE_INPUT]; //  파일에서 한 줄씩 읽기 위한 배열
  char opcode[OPCODE];  //  opcode 저장용 배열
  char mnemonic[MNEMONIC];  //  mnemonic 저장용 배열
  char format[FORMAT];  //  format 저장용 배열

  int i;
  int  mstart, mend;  //  mnemonic start, end
  int fstart;   //  format start
  
  opcode[OPCODE - 1] = '\0';
  mnemonic[MNEMONIC - 1] = '\0';

  while(TRUE) {
    if(!fgets(input, OPCODE_INPUT, fp)) break;

    for(i = 0; i < 2; i++)
      opcode[i] = input[i]; //  opcode 저장
    for(i = 2; ; i++) {
      if(input[i] != ' ' && input[i] != '\t') {
        mstart = i;
        break;
      }
    }

    for(i = mstart; ; i++) {
      if(input[i] == ' ' || input[i] == '\t') {
        mnemonic[i - mstart] = '\0';
        mend = i - 1;
        break;
      }
      mnemonic[i - mstart] = input[i]; //  mnemonic 저장
    }

    for(i = mend + 1; ; i++) {
      if(input[i] != ' ' && input[i] != '\t') {
        fstart = i;
        break;
      }
    }

    for(i = fstart; ; i++) {
      if(input[i] == ' ' || input[i] == '\t') {
        format[i - fstart] = '\0';
        break;
      }
      format[i - fstart] = input[i];
    }
    
    push_node(opcode, mnemonic, format);  //  opcode와 mnemonic 저장 후 optable에 삽입
  }

  fclose(fp);
}
