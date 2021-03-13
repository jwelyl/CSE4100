#include "command.h"
#include "memory.h"
#include "queue.h"
#include "optable.h"

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
int check_no_opt(char* input, int start) {
  if(input[start] == '\0') return TRUE;
  
  int i = start;
  for(; i < INPUT_LEN; i++) {
    if(input[i] == '\0') break;
    else if(input[i] == ' ') continue;
    else return FALSE;
  }

  return TRUE;
}

/*
 * du[mp], e[dit], f[ill], opcode
 * 명령어 다음 option 확인
 */

//  du[mp] [start, end]에서 start, end 결정
int check_dump(char* input, char* cmd, int* from, int* to, char* opt1, char* opt2) {
  int i = !strcmp(cmd, "dump") ? 4 : 2;

  int comma = FALSE;  //  ',' 발견 여부
  int sf = NONE, st = NONE; //  start 옵션의 시작, 끝 index
  int ef = NONE, et = NONE; //  end 옵션의 시작, 끝 index
  int hex;
 
  //  du[mp]AA 꼴의 명령어는 du[mp]로 인식되지 않음
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

//  opcode 명령 option 찾기
int check_opcode(char* input, char* mnemonic, char* opcode) {
  int i;
  int ms = NONE, me = NONE; //  mnemonic 옵션의 존재 여부 check(mnemonic 시작 index, 끝 index)
 
  // 
//  printf("input : %s\n", input);
  //
/*
  printf("[");
  for(i = 0; i < MNEMONIC; i++) {
    printf("%c, ", mnemonic[i]);
  }
  printf("]\n");
*/

  if(input[6] == '\0') {  //  mnemonic 없이 명령이 끝날 경우
//    printf("mnemonic 없이 명령 끝\n");
    return FALSE;
  }

  for(i = 7; i < INPUT_LEN; i++) {
    if(input[i] == '\0') {
      if(ms == NONE && me == NONE) {  //  mnemonic option을 발견하지 못하고 명령이 끝난 경우
//        printf("mnemonic option 발견 x\n");
        return FALSE;
      }
      else if(ms != NONE && me == NONE) { //  mnemonic option 후에 바로 명령이 끝난 경우
//        printf("mnemnoic 끝나자마자 명령 끝\n");
        me = i - 1;
//        printf("me = %d\n", me);
        break;
      }
      else if(ms != NONE && me != NONE) {  //  mnemonic option 후에 ' '만 존재하고 명령이 끝난 경우
//        printf("mnemonic 이후 공백만 존재하다 명령 끝\n");
        break;
      }
    } //  if-'\0' end
    
    else if(input[i] != ' ' && (input[i] < 'A' || 'Z' < input[i])) {  //  유효하지 않은 mnemonic
//        printf("유효하지 않은 mnemonic 입력\n");
        return FALSE;
    }

    else if(ms == NONE && ('A' <= input[i] && input[i] <= 'Z')) { //  mnemonic start index 찾음
        ms = i;
//        printf("ms = %d\n", ms);
        continue;
    }

    else if(ms != NONE && me == NONE) { //  mnemonic start index를 찾은 상태에서
      if(input[i] == ' ') {
//        printf("mnemonic 끝난 뒤에 공백 존재\n");
        me = i - 1;
//        printf("me = %d\n", me);
        continue;
      }
      else if('A' <= input[i] && input[i] <= 'Z') continue;;
    }

    else if(me != NONE) {
      if(input[i] == ' ') continue;
      else if('A' <= input[i] && input[i] <= 'Z') {
//        printf("mnemonic 끝난 뒤에 불필요한 옵션 존재\n");
        return FALSE;
      }
    }
  } //  for-end

 
  printf("찾아낸 mnemonic 길이 : %d\n", me - ms + 1);
  if(me - ms + 1 > MNEMONIC - 1) {  //  mnemonic option 길이가 최대 MNEMONIC 길이보다 길 경우
//    printf("mnemonic 길이 초과\n");
    return FALSE;
  }

  for(i = ms; i <= me; i++) { //  mnemonic 배열에 mnemonic 옵션 복사
    mnemonic[i - ms] = input[i];
  }
//  printf("[%d]\n", i);
  mnemonic[i] = '\0';

//  printf("찾아낸 유효한 mnemonic : %s\n", mnemonic);

  if(!find_opcode(mnemonic, opcode)) {
    printf("해당 mnemonic이 존재하지 않음\n");
    return FALSE;
  }

  printf("opcode is %s\n", opcode);
  return TRUE;
}

//  e[dit] address, value에서 address, value 결정
int check_edit(char* input, char* cmd, int* addr, int* val, char* opt1, char* opt2) {
  int i = !strcmp(cmd, "edit") ? 4 : 1;

  int comma = FALSE;  //  ',' 발견 여부
  int af = NONE, at = NONE; //  address 옵션의 시작, 끝 index
  int vf = NONE, vt = NONE; //  value 옵션의 시작, 끝 index
  int hex;

  //  e[dit]AA 꼴의 명령어는 e[dit]로 인식되지 않음
  if(input[i] == '\0') {  //  option 없이 e[dit] 꼴
    printf("address, value 옵션이 필요함.\n");
    return FALSE;
  }
  else {  //  e[dit]... 꼴
    i++;
    
    for(; i < INPUT_LEN; i++) {
      if(input[i] == '\0') {
        if(af == NONE || at == NONE || vf == NONE) {  //  필요한 option이 없을 경우
          printf("option 부족\n");
          return FALSE;
        }
        else if(vf != NONE && vt == NONE) {  //  value option 다음에 명령이 끝날 경우
          vt = i - 1;
          printf("vt = %d\n", vt);
        }

        break;
      } //  if '\0' end 

      if(af == NONE && input[i] != ' ') { //  address option 시작 부분 발견
        af = i;
        printf("af = %d\n", af);
      }

      else if(af != NONE && at == NONE) {  //  address option 끝 부분 찾는 중
        if(input[i] == ' ' || input[i] == ',') {  //  address option 끝 부분 발견
          if(input[i] == ',') comma = TRUE;

          at = i - 1;
          printf("at = %d\n", at);
        }
      }

      else if(at != NONE && !comma) { //  address option 찾은 뒤 ',' 찾는 중
        if(input[i] != ',' && input[i] != ' ') {  //  ',' 이전에 value option이 나올 경우
          printf(", 이전에 value 옵션 나옴\n");
          return FALSE;
        }
        else if(input[i] == ',') {  //  ','를 찾은 경우
          comma = TRUE;
          printf("%d에서 comma 발견\n", i);
        }
      }
      
      else if(comma && vf == NONE) {  // ',' 찾은 뒤 value option 찾는 중
        if(input[i] != ' ') { //  value option 시작 부분 발견
          vf = i;
          printf("vf = %d\n", vf);
        }
      }

      else if(vf != NONE && vt == NONE) { //  value option 끝 부분 찾는 중
        if(input[i] == ' ') { //  end option 끝 부분 발견
          vt = i - 1;
          printf("vt = %d\n", vt);
        }
      }
      else if(vt != NONE) { 
        if(input[i] != ' ') { //  end option 이후 다른 option 발견
          printf("end option 이후 다른 option 발견\n");
          return FALSE;
        }
      }
    } //  for end
  } //  else end

  if(af == NONE || at == NONE || vf == NONE || vt == NONE || !comma) {
    printf("edit 명령에서 뭔가 잘못되었음.\n");
    return FALSE;
  }

  if(at - af + 1 < 0 || at - af + 1 > 5) {  //  address option 길이 확인
    printf("address 길이 문제\n");
    return FALSE;
  }

  if(vt - vf + 1 < 0 || vt - vf + 1 > 2) {  //  value option 길이 확인
    printf("value option 길이 문제\n");
    return FALSE;
  }

  for(i = af; i <= at; i++) {
    opt1[i - af] = input[i];
  }
  opt1[i] = '\0';

  for(i = vf; i <= vt; i++) {
    opt2[i - vf] = input[i];
  }
  opt2[i] = '\0';

  printf("address : [%s]\n", opt1);
  printf("value : [%s]\n", opt2);

  *addr = 0;
  *val = 0;

  hex = 1;
  for(i = at - af; i >= 0; i--) {
    if('0' <= opt1[i] && opt1[i] <= '9')
      *addr += (hex * (opt1[i] - '0'));
    else if('A' <= opt1[i] && opt1[i] <= 'F')
      *addr += (hex * (opt1[i] - 'A' + 10));
    else if('a' <= opt1[i] && opt1[i] <= 'f')
      *addr += (hex * (opt1[i] - 'a' + 10));
    else {  //  잘못된 address
      printf("잘못된 address 위치 : %d\n", i);
      printf("잘못된 address(%c) 포함\n", opt1[i]); 
      return FALSE;
    }
    hex *= 16;
  }

  hex = 1;
  for(i = vt - vf; i >= 0; i--) {
    if('0' <= opt2[i] && opt2[i] <= '9')
      *val += (hex * (opt2[i] - '0'));
    else if('A' <= opt2[i] && opt2[i] <= 'F')
      *val += (hex * (opt2[i] - 'A' + 10));
    else if('a' <= opt2[i] && opt2[i] <= 'f')
      *val += (hex * (opt2[i] - 'a' + 10));
    else {  //  잘못된 address
      printf("잘못된 value 위치 : %d\n", i);
      printf("잘못된 value(%c) 포함\n", opt2[i]); 
      return FALSE;
    }
    hex *= 16;
  }

  printf("address : <%d>\nvalue : <%d>\n", *addr, *val);
  return TRUE;
}

int process_command(char* cmd, char* input) { //  qu[it] 명령 수행 시 FALSE 반환(프로그램 종료)
  DIR* dp = NULL;
  struct dirent* dir_entry;
  struct stat dir_stat;
  char opt1[MAX_OPR] = {0, }, opt2[MAX_OPR] = {0, }, opt3[MAX_OPR] = {0, };
  char mnemonic[MNEMONIC] = {0, }, opcode[OPCODE] = {0, };
 
  //  q[uit]
  if(!strcmp(cmd, "quit") || !strcmp(cmd, "q")) {
    if(!strcmp(cmd, "quit")) {
      if(!check_no_opt(input, 4)) return TRUE;
    } 
    else {
      if(!check_no_opt(input, 1)) return TRUE;
    }
    delete_queue();
    delete_optable();
    return FALSE;
  }
 
  //  h[elp]
  if(!strcmp(cmd, "help") || !strcmp(cmd, "h")) {
    
    if(!strcmp(cmd, "help")) {
      if(!check_no_opt(input, 4)) return TRUE;
    } 
    else {
      if(!check_no_opt(input, 1)) return TRUE;
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
      if(!check_no_opt(input, 3)) return TRUE;
    } 
    else {
      if(!check_no_opt(input, 1)) return TRUE;
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
      if(!check_no_opt(input, 7)) return TRUE;
    } 
    else {
      if(!check_no_opt(input, 2)) return TRUE;
    }

    enqueue(input);
    print_queue();
  }

  //  reset
  else if(!strcmp(cmd, "reset")) {
    if(!check_no_opt(input, 5)) return TRUE;
    reset_memory();
    enqueue(input);
  }

  //  opcodelist
  else if(!strcmp(cmd, "opcodelist")) {
    
     if(!check_no_opt(input, 10)) return TRUE;

     print_optable();
     enqueue(input);
  }
  
  //  option 필요한 명령어
  //  du[mp]
  else if(!strcmp(cmd, "dump") || !strcmp(cmd, "du")) {
    int from, to;

    if(!check_dump(input, cmd, &from, &to, opt1, opt2)) {
      return TRUE;
    }

    printf("from %d to %d\n", from, to); 
    enqueue(input);
  }
  //  e[dit]
  else if(!strcmp(cmd, "edit") || !strcmp(cmd, "e")) {
    int addr, val;

    if(!check_edit(input, cmd, &addr, &val, opt1, opt2)) {
      printf("e[dit] 명령어 어딘가 잘못됨.\n");
      return TRUE;  
    }

    edit_value(addr, val);
    enqueue(input);
  }
  //  f[ill]
  else if(!strcmp(cmd, "fill") || !strcmp(cmd, "f")) {
    enqueue(input);
    printf("%s는 구현 예정\n", cmd);
  }
  //  opcode
  else if(!strcmp(cmd, "opcode")) {
    if(!check_opcode(input, mnemonic, opcode)) return TRUE;
    enqueue(input);
  }

  return TRUE;
}

