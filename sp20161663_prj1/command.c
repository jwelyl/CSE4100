#include "command.h"
#include "queue.h"
#include "optable.h"

//  du[mp] 명령어 주소
int address = 0;

void clearInputBuffer() {
  while(getchar() != '\n');
}

int invalid_command(char* input, char* cmd) {
  int i;
  int inputOver = TRUE;
  int cmdLenOver = FALSE;
  
  for(i = 0; i < INPUT_LEN; i++) {
    if(input[i] == '\n') {
      input[i] = '\0';
      inputOver = FALSE;
      break;
    }
  } 

  if(inputOver) clearInputBuffer();

  i = 0;
  while(TRUE) {
    if(input[i] == ' ' || input[i] == '\0') {
      cmd[i] = '\0';
      break;
    }
    else if(i == MAX_CMD - 1) {
      cmdLenOver = TRUE;
      cmd[i] = '\0';
      break;
    }
    cmd[i] = input[i];
    i++;
  }

  if(cmdLenOver) return TRUE;

  if(!strcmp(cmd, "help") || !strcmp(cmd, "h") || !strcmp(cmd, "dir") || !strcmp(cmd, "d") || 
     !strcmp(cmd, "quit") || !strcmp(cmd, "q") || !strcmp(cmd, "history") || !strcmp(cmd, "hi") || 
     !strcmp(cmd, "dump") || !strcmp(cmd, "du") || !strcmp(cmd, "edit") || !strcmp(cmd, "e") || 
     !strcmp(cmd, "fill") || !strcmp(cmd, "f") || !strcmp(cmd, "reset") || !strcmp(cmd, "opcode") || 
     !strcmp(cmd, "opcodelist")) {
      
     return FALSE;
  }
  else return TRUE;
}

/* 
 * h[elp], d[ir], q[uit], hi[story], reset, opcodelist
 * 명령어만 입력받았는지 확인
 * 불필요한 옵션이 있을 경우 FALSE 반환
 */
int checkNoOpt(char* input, int start) {
  if(input[start] == '\0') return TRUE;
  
  int i = start;
  for(; i < INPUT_LEN; i++) {
    if(input[i] == '\0') return TRUE;
    else if(input[i] == ' ') continue;
    else return FALSE;
  }
}

/*
 * du[mp], e[dit], f[ill], opcode
 * 명령어 다음 option 확인
 */

//  du[mp] [start, end]에서 start, end 결정
int checkDump(char* input, char* cmd, int* from, int* to, char* opt1, char* opt2) {
  int i = !strcmp(cmd, "dump") ? 4 : 2;

  int comma = FALSE;  //  ',' 발견 여부
  int sf = NONE, st = NONE; //  start 옵션의 시작, 끝 index
  int ef = NONE, et = NONE; //  end 옵션의 시작, 끝 index
  int hex;
 
  //  du[mp]AA 꼴의 명령어는 du[mp]로 인식되지 않
  if(input[i] == '\0') { //  option 없이 du[mp] 꼴
    *from = address;
    *to = *from + 159;

    return TRUE;
  }
  else {  //  du[mp]... 꼴
    i++;

    for(; i < INPUT_LEN; i++) {
      if(input[i] == '\0') {
        //  option 바로 다음에 NULL이 올 경우
        if(sf != NONE && st == NONE) {
          st = i - 1;
          opt1[i - sf] = '\0';
        }
        else if(ef != NONE && et == NONE) {
          et = i - 1;
          opt2[i - ef] = '\0';
        }
        break;
      } //  if '\0' end

      if(sf == NONE && input[i] != ' ') { //  start option 시작 부분 발견
        sf = i;
        opt1[i - sf] = input[i];
      }
      else if(sf != NONE && st == NONE) { //  start option 끝 부분 찾는 중
        if(input[i] == ' ' || input[i] == ',') {  //  start option 끝 부분 발견
          if(input[i] == ',') comma = TRUE;

          st = i - 1;
          opt1[i - sf] = '\0';
        }
        else opt1[i - sf] = input[i];
      }
      else if(st != NONE && !comma) { //  start option 찾은 뒤 ',' 찾는 중
        if(input[i] != ',' && input[i] != ' ') { // ',' 이전에 end option이 나올 경우
          return FALSE;
        }
        else if(input[i] == ',') { // ','를 찾은 경우
          comma = TRUE;
        }
      }
      else if(comma && ef == NONE) {  //  ',' 찾은 뒤 end option 찾는 중
        if(input[i] != ' ') { //  end option 시작 부분 발견
          ef = i;
          opt2[i - ef] = input[i];
        }
      }
      else if(ef != NONE && et == NONE) { //  end option 끝 부분 찾는 중
        if(input[i] == ' ') { //  end option 끝 부분 발견
          et = i - 1;
          opt2[i - ef] = '\0';
        }
        else {
          opt2[i - ef] = input[i];
        }
      }
      else if(et != NONE) {
        if(input[i] != ' ') { //  end option 이후 다른 option 발견
          return FALSE;
        }
      }
    } //  for - end
  } //  else - end

  if(comma && ef == NONE)  //  ',' 후에 end option이 없을 경우
    return FALSE;
  
  if(sf == NONE && st == NONE && ef == NONE && et == NONE) {
    //  start, end 옵션 모두 없을 경우
    *from = address;
    *to = address + 159;
  }
  else {
    if(sf != NONE && st != NONE) {
      //  start 옵션이 있을 경우
      hex = 1;
      *from = 0;
      for(i = st - sf; i >= 0; i--) {
        if('0' <= opt1[i] && opt1[i] <= '9')
          *from += (hex * (opt1[i] - '0'));
        else if('A' <= opt1[i] && opt1[i] <= 'F') 
          *from += (hex * (opt1[i] - 'A' + 10));
        else if('a' <= opt1[i] && opt1[i] <= 'f')
          *from += (hex * (opt1[i] - 'a' + 10));
        else {  //  잘못된 start option
          return FALSE;  
        }
        hex *= 16;
      }
    }
    if(sf != NONE && st != NONE && ef == NONE && et == NONE) 
      *to = *from + 159;
    
    else if(sf != NONE && st != NONE && ef != NONE && et != NONE) { 
      // end 옵션 있을 경우

      hex = 1;
      *to = 0;
      for(i = et - ef; i >= 0; i--) {
        if('0' <= opt2[i] && opt2[i] <= '9')
          *to += (hex * (opt2[i] - '0'));
        else if('A' <= opt2[i] && opt2[i] <= 'F') 
          *to += (hex * (opt2[i] - 'A' + 10));
        else if('a' <= opt2[i] && opt2[i] <= 'f')
          *to += (hex * (opt2[i] - 'a' + 10));
        else {  //  잘못된 end option
          return FALSE;
        }  
        hex *= 16;
      }
    
      if(*to < *from)  //  [start, end] 범위가 잘못됨
        return FALSE;
    } 
  }

  return TRUE;
}

int check_opcode(char* input) {
  char mnemonic[MNEMONIC];
  char opcode[OPCODE];
  int i;
  int ms = NONE, me = NONE; //  mnemonic 옵션의 존재 여부

  if(input[6] == '\0')  // opcode 명령어에 mnemonic 옵션 없음
    return FALSE;
  for(i = 7; i < INPUT_LEN; i++) {
    printf("[[ input[%d] : %c ]] \n", i, input[i]);

    if(input[i] != ' ' && input[i] != '\0' && (input[i] < 'A' || input[i] > 'Z')) {//  유효하지 않은 mnemonic 옵션
      //
      printf("유효하지 않은 mnemonic 옵션 입력 input[%d] = %c\n", i, input[i]);
      //
      return FALSE;
    }

    if(ms != NONE && me != NONE && input[i] == '\0') break;
    else if(ms != NONE && me == NONE && input[i] == '\0') {
      me = i - 1;
      mnemonic[i] = '\0';
      break;
    }
    else if(ms == NONE && input[i] == '\0') {
      //
      printf("mnemonic 옵션 나오기 전에 명령어 끝\n");
      //
      return FALSE;
    }
    
    else if(ms == NONE && ('A' <= input[i] && input[i] <= 'Z')) {
      ms = i;
      mnemonic[i - ms] = input[i];
    }

    else if((ms != NONE && me == NONE) && ('A' <= input[i] && input[i] <=  'Z')) 
      mnemonic[i - ms] = input[i];
    
    else if((ms != NONE && me == NONE) && input[i] == ' ') {
      me = i - 1;
      mnemonic[i] = '\0'; 
    }

    else if((ms != NONE && me != NONE) && input[i] != ' ') { //  mnemonic 이외의 불필요한 옵션
      printf("불필요한 옵션\n");
      return FALSE;
    }
  } // for문 종료
  
  printf("ms = %d\nme = %d\n", ms, me);
  printf("[{%s}]\n", mnemonic);
  if(!find_opcode(mnemonic, opcode)) {
    printf("해당 mnemonic의 opcode가 존재하지 않음\n");
    return FALSE;
  }
  printf("opcode is %s\n", opcode);
  return TRUE;
}

int process_command(char* cmd, char* input) { //  qu[it] 명령 수행 시 FALSE 반환(프로그램 종료)
  DIR* dp = NULL;
  struct dirent* dir_entry;
  struct stat dir_stat;
  char opt1[MAX_OPR], opt2[MAX_OPR], opt3[MAX_OPR];
  char mnemonic[MNEMONIC];
 
  //  q[uit]
  if(!strcmp(cmd, "quit") || !strcmp(cmd, "q")) {
    if(!strcmp(cmd, "quit")) {
      if(!checkNoOpt(input, 4)) return TRUE;
    } 
    else {
      if(!checkNoOpt(input, 1)) return TRUE;
    }
    deleteQueue();
    delete_optable();
    return FALSE;
  }
 
  //  h[elp]
  if(!strcmp(cmd, "help") || !strcmp(cmd, "h")) {
    
    if(!strcmp(cmd, "help")) {
      if(!checkNoOpt(input, 4)) return TRUE;
    } 
    else {
      if(!checkNoOpt(input, 1)) return TRUE;
    }
          
    enqueue(input);

    printf("\n\th[elp]");
    printf("\n\td[ir]");
    printf("\n\tq[uit]");
    printf("\n\thi[story]");
    printf("\n\tdu[mp] [start, end]");
    printf("\n\te[dit] address, value");
    printf("\n\tf[ill] start, end, value");
    printf("\n\treset");
    printf("\n\topcode mnemonic");
    printf("\n\topcodelist\n\n");
  }

  //  d[ir]
  else if(!strcmp(cmd, "dir") || !strcmp(cmd, "d")) {
    
    if(!strcmp(cmd, "dir")) {
      if(!checkNoOpt(input, 3)) return TRUE;
    } 
    else {
      if(!checkNoOpt(input, 1)) return TRUE;
    }

    enqueue(input);

    if((dp = opendir(".")) == NULL) {
      printf("현재 디렉토리를 열 수가 없습니다.\n");
      exit(-1);
    }

    while((dir_entry = readdir(dp))) {
      lstat(dir_entry->d_name, &dir_stat);

      if(S_ISDIR(dir_stat.st_mode))
        printf("%s/  ", dir_entry->d_name);
      else {
        if((dir_stat.st_mode) & S_IXUSR)
          printf("%s*  ", dir_entry->d_name);
        else
          printf("%s  ", dir_entry->d_name);
      }
    }
    printf("\n");
  }

  //  hi[story]
  else if(!strcmp(cmd, "history") || !strcmp(cmd, "hi")) {
    
    if(!strcmp(cmd, "history")) {
      if(!checkNoOpt(input, 7)) return TRUE;
    } 
    else {
      if(!checkNoOpt(input, 2)) return TRUE;
    }

    enqueue(input);
    printQueue();
  }

  //  reset
  else if(!strcmp(cmd, "reset")) {
    
    if(!checkNoOpt(input, 5)) return TRUE;

    enqueue(input);
    printf("%s는 구현 예정\n", cmd);
  }

  //  opcodelist
  else if(!strcmp(cmd, "opcodelist")) {
    
     if(!checkNoOpt(input, 10)) return TRUE;

     print_optable();
     enqueue(input);
     // printf("%s는 구현 예정\n", cmd);
  }
  
  //  option 필요한 명령어
  //  du[mp]
  else if(!strcmp(cmd, "dump") || !strcmp(cmd, "du")) {
    int from, to;

    if(!checkDump(input, cmd, &from, &to, opt1, opt2)) {
      return TRUE;
    }
    enqueue(input);
  }
  //  e[dit]
  else if(!strcmp(cmd, "edit") || !strcmp(cmd, "e")) {
    enqueue(input);
    printf("%s는 구현 예정\n", cmd);
  }
  //  f[ill]
  else if(!strcmp(cmd, "fill") || !strcmp(cmd, "f")) {
    enqueue(input);
    printf("%s는 구현 예정\n", cmd);
  }
  //  opcode
  else if(!strcmp(cmd, "opcode")) {
    if(!check_opcode(input)) return TRUE;
    
    printf("here?\n"); 
    enqueue(input);
    printf("there?\n");
    printf("%s는 구현 예정\n", cmd);
  }

  return TRUE;
}

