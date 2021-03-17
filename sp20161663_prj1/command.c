#include "command.h"
#include "memory.h"
#include "queue.h"
#include "optable.h"

int address = 0;

void clear_input_buffer() {
  while(getchar() != '\n');
}

int invalid_command(char* input, char* cmd) {
  int i;
  int cmd_start = NONE, cmd_end = NONE;
  int input_over = TRUE;
  int cmd_len_over = FALSE;
  
  for(i = 0; i < INPUT_LEN; i++) {  //  입력받은 문장에서 '\n' 제거
    if(input[i] == '\n') {
      input[i] = '\0';
      input_over = FALSE;
      break;
    }
  } 

  if(input_over) clear_input_buffer();

  i = 0;
  while(TRUE) {
    if(input[i] == ' ' || input[i] == '\0') {
      cmd[i] = '\0';
      break;
    }
    else if(i == MAX_CMD - 1) {
      cmd_len_over = TRUE;
      cmd[i] = '\0';
      break;
    }
    cmd[i] = input[i];
    i++;
  }

  if(cmd_len_over) {
    printf("Invalid command\n");
    return TRUE;
  }

  if(!strcmp(cmd, "help") || !strcmp(cmd, "h") || !strcmp(cmd, "dir") || !strcmp(cmd, "d") || 
     !strcmp(cmd, "quit") || !strcmp(cmd, "q") || !strcmp(cmd, "history") || !strcmp(cmd, "hi") || 
     !strcmp(cmd, "dump") || !strcmp(cmd, "du") || !strcmp(cmd, "edit") || !strcmp(cmd, "e") || 
     !strcmp(cmd, "fill") || !strcmp(cmd, "f") || !strcmp(cmd, "reset") || !strcmp(cmd, "opcode") || 
     !strcmp(cmd, "opcodelist")) {
      
 //   printf("cmd : %s\n", cmd);
     return FALSE;
  }
  else {  //  존재하지 않는 명령어 또는 띄어쓰기 없이 바로 option 입력
    printf("Invalid command\n");
    return TRUE;
  }
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
int check_dump(char* input, char* cmd, int* start, int* end, char* opt1, char* opt2) {
  int i = !strcmp(cmd, "dump") ? 4 : 2;

  int comma = FALSE;  //  ',' 발견 여부
  int sf = NONE, st = NONE; //  start 옵션의 시작, 끝 index
  int ef = NONE, et = NONE; //  end 옵션의 시작, 끝 index
  int hex;
 
  //  du[mp]AA 꼴의 명령어는 du[mp]로 인식되지 않음
  if(input[i] == '\0') { //  option 없이 du[mp] 꼴
    if(address >= MAX_MEM_SIZE) { //  메모리 끝까지 출력했을 경우
  //    printf("End of memory\n");
  //    return FALSE;
        address = 0;  //  처음부터 다시 출력함
    }
    
    *start = address;
    *end = *start + 159;

    if(*end >= MAX_MEM_SIZE) {
      *end = MAX_MEM_SIZE - 1;
    }
    address = *end + 1;

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

  if(comma && ef == NONE) {  //  ',' 후에 end option이 없을 경우
    return FALSE;
  }
  
  if(sf == NONE && st == NONE && ef == NONE && et == NONE) {
    //  start, end 옵션 모두 없을 경우
   if(address >= MAX_MEM_SIZE) {
//      printf("End of memory\n");
//      return FALSE;
        address = 0;
    }
    
    *start = address;
    *end = *start + 159;

    if(*end >= MAX_MEM_SIZE) {
      *end = MAX_MEM_SIZE - 1;
    }
    address = *end + 1;

    return TRUE;
  }
  else {
    if(sf != NONE && st != NONE) {
      //  start 옵션이 있을 경우
      hex = 1;
      *start = 0;
      for(i = st - sf; i >= 0; i--) {
        if('0' <= opt1[i] && opt1[i] <= '9')
          *start += (hex * (opt1[i] - '0'));
        else if('A' <= opt1[i] && opt1[i] <= 'F') 
          *start += (hex * (opt1[i] - 'A' + 10));
        else if('a' <= opt1[i] && opt1[i] <= 'f')
          *start += (hex * (opt1[i] - 'a' + 10));
        else {  //  잘못된 start option
          return FALSE;  
        }
        hex *= 16;
      }
      if(strlen(opt1) > 5)  //  00000 ~ FFFFF 범위를 초과할 경우 
        return FALSE;
    }
    if(sf != NONE && st != NONE && ef == NONE && et == NONE) {
      //  start option만 있을 경우
      *end = *start + 159;
      if(*end >= MAX_MEM_SIZE) 
        *end = MAX_MEM_SIZE - 1;
    }

    else if(sf != NONE && st != NONE && ef != NONE && et != NONE) { 
      // end option도 있을 경우

      hex = 1;
      *end = 0;
      for(i = et - ef; i >= 0; i--) {
        if('0' <= opt2[i] && opt2[i] <= '9')
          *end += (hex * (opt2[i] - '0'));
        else if('A' <= opt2[i] && opt2[i] <= 'F') 
          *end += (hex * (opt2[i] - 'A' + 10));
        else if('a' <= opt2[i] && opt2[i] <= 'f')
          *end += (hex * (opt2[i] - 'a' + 10));
        else   //  잘못된 end option
          return FALSE;
        hex *= 16;
      }
      if(strlen(opt2) > 5) //   00000 ~ FFFFF 범위를 초과할 경우
        return FALSE;

      if(*end < *start)  //  [start, end] 범위가 잘못됨
        return FALSE;
    } 
  }

  return TRUE;
}

//  opcode 명령 option 찾기
int check_opcode(char* input, char* mnemonic, char* opcode) {
  int i;
  int ms = NONE, me = NONE; //  mnemonic 옵션의 존재 여부 check(mnemonic 시작 index, 끝 index)

  if(input[6] == '\0')   //  mnemonic 없이 명령이 끝날 경우
    return FALSE;
  
  for(i = 7; i < INPUT_LEN; i++) {
    if(input[i] == '\0') {
      if(ms == NONE && me == NONE)   //  mnemonic option을 발견하지 못하고 명령이 끝난 경우
        return FALSE;
      else if(ms != NONE && me == NONE) { //  mnemonic option 후에 바로 명령이 끝난 경우
        me = i - 1;
        break;
      }
      else if(ms != NONE && me != NONE)   //  mnemonic option 후에 ' '만 존재하고 명령이 끝난 경우
        break;
    } //  if-'\0' end
    
    else if(input[i] != ' ' && (input[i] < 'A' || 'Z' < input[i]))   //  유효하지 않은 mnemonic
        return FALSE;

    else if(ms == NONE && ('A' <= input[i] && input[i] <= 'Z')) { //  mnemonic start index 찾음
        ms = i;
        continue;
    }

    else if(ms != NONE && me == NONE) { //  mnemonic start index를 찾은 상태에서
      if(input[i] == ' ') {
        me = i - 1;
        continue;
      }
      else if('A' <= input[i] && input[i] <= 'Z') continue;;
    }

    else if(me != NONE) {
      if(input[i] == ' ') continue;
      else if('A' <= input[i] && input[i] <= 'Z') 
        return FALSE;
    }
  } //  for-end
 
  if(me - ms + 1 > MNEMONIC - 1)   //  mnemonic option 길이가 최대 MNEMONIC 길이보다 길 경우
    return FALSE;

  for(i = ms; i <= me; i++)  //  mnemonic 배열에 mnemonic 옵션 복사
    mnemonic[i - ms] = input[i];
  mnemonic[i] = '\0';

  if(!find_opcode(mnemonic, opcode)) 
    return FALSE;

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
  if(input[i] == '\0')   //  option 없이 e[dit] 꼴
    return FALSE;
  
  else {  //  e[dit]... 꼴
    i++;
    
    for(; i < INPUT_LEN; i++) {
      if(input[i] == '\0') {
        if(af == NONE || at == NONE || vf == NONE)   //  필요한 option이 없을 경우
          return FALSE;
        
        else if(vf != NONE && vt == NONE)   //  value option 다음에 명령이 끝날 경우
          vt = i - 1;

        break;
      } //  if '\0' end 

      if(af == NONE && input[i] != ' ')  //  address option 시작 부분 발견
        af = i;

      else if(af != NONE && at == NONE) {  //  address option 끝 부분 찾는 중
        if(input[i] == ' ' || input[i] == ',') {  //  address option 끝 부분 발견
          if(input[i] == ',') comma = TRUE;

          at = i - 1;
        }
      }

      else if(at != NONE && !comma) { //  address option 찾은 뒤 ',' 찾는 중
        if(input[i] != ',' && input[i] != ' ')   //  ',' 이전에 value option이 나올 경우
          return FALSE;
        else if(input[i] == ',')   //  ','를 찾은 경우
          comma = TRUE;
      }
      
      else if(comma && vf == NONE) {  // ',' 찾은 뒤 value option 찾는 중
        if(input[i] != ' ')  //  value option 시작 부분 발견
          vf = i;
      }

      else if(vf != NONE && vt == NONE) { //  value option 끝 부분 찾는 중
        if(input[i] == ' ')  //  value option 끝 부분 발견
          vt = i - 1;
      }
      else if(vt != NONE) { 
        if(input[i] != ' ')  //  value option 이후 다른 option 발견
          return FALSE;
      }
    } //  for end
  } //  else end

  if(af == NONE || at == NONE || vf == NONE || vt == NONE || !comma) 
    return FALSE;

  if(at - af + 1 < 0 || at - af + 1 > 5)   //  address option 길이 확인
    return FALSE;
  
  if(vt - vf + 1 < 0 || vt - vf + 1 > 2)   //  value option 길이 확인
    return FALSE;

  for(i = af; i <= at; i++) {
    opt1[i - af] = input[i];
  }
  opt1[i] = '\0';

  for(i = vf; i <= vt; i++) {
    opt2[i - vf] = input[i];
  }
  opt2[i] = '\0';

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
    else   //  잘못된 address
      return FALSE;
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
    else   //  잘못된 value
      return FALSE;
    hex *= 16;
  }

  return TRUE;
}

//  f[ill] start, end, value에서 start, end, value 결정
int check_fill(char* input, char* cmd, int* start, int* end, int* value,
  char* opt1, char* opt2, char* opt3) {
  int i = !strcmp(cmd, "fill") ? 4 : 1;

  int comma1 = FALSE, comma2 = FALSE; //  ',' 2개 발견 여부
  int sf = NONE, st = NONE; //  start 옵션의 시작, 끝 index
  int ef = NONE, et = NONE; //  end 옵션의 시작, 끝 index
  int vf = NONE, vt = NONE; //  value 옵션의 시작, 끝 index
  int hex;

  //  f[ill]AA 꼴의 명령어는 f[ill]로 인식되지 않음
  if(input[i] == '\0')   //  option 없이 f[ill] 꼴
    return FALSE;
  else {  //  f[ill]... 꼴
    i++;

    for(; i < INPUT_LEN; i++) {
      if(input[i] == '\0') {
        if(sf == NONE || st == NONE || ef == NONE || 
            et == NONE || vf == NONE) //  필요한 option이 없을 경우
          return FALSE;
        else if(vf != NONE && vt == NONE)  //  value option 다음에 명령이 끝날 경우
          vt = i - 1;

        break;
      } //  if '\0' end

      if(sf == NONE && input[i] != ' ')  //  start option 시작 부분 발견
        sf = i;

      else if(sf != NONE && st == NONE) { //  start option 끝 부분 찾는 중
        if(input[i] == ' ' || input[i] == ',') {  //  start option 끝 부분 발견
          if(input[i] == ',') 
            comma1 = TRUE;

          st = i - 1;
        }
      }

      else if(st != NONE && !comma1) {  //  start option 찾은 뒤 ',' 찾는 중
        if(input[i] != ',' && input[i] != ' ')   //  첫번째 ',' 이전에 end option이 나올 경우
          return FALSE;  
        else if(input[i] == ',')   //  첫번째 ','를 찾은 경우
          comma1 = TRUE;
      }

      else if(comma1 && ef == NONE) { //  첫번째 ',' 찾은 뒤 end option 찾는 중
        if(input[i] != ' ')  //  end option 시작 부분 발견
          ef = i;
      }

      else if(ef != NONE && et == NONE) { //  end option 끝 부분 찾는 중
        if(input[i] == ' ' || input[i] == ',') {  //  end option 끝 부분 발견
          if(input[i] == ',') 
            comma2 = TRUE;

          et = i - 1;
        }
      }

      else if(et != NONE && !comma2) {  //  end option 찾은 뒤 ',' 찾는 중
        if(input[i] != ',' && input[i] != ' ')   //  두번째 ',' 이전에 value option이 나올 경우
          return FALSE;  
        else if(input[i] == ',')   //  두번째 ','를 찾은 경우
          comma2 = TRUE;
      }

      else if(comma2 && vf == NONE) { //  두 번째 ',' 찾은 뒤 value option 찾는 중
        if(input[i] != ' ')  //  value option 시작 부분 발견
          vf = i;
      }

      else if(vf != NONE && vt == NONE) { //  value option 끝 부분 찾는 중
        if(input[i] == ' ')  //  value option 끝 부분 발견
          vt = i - 1;
      }

      else if(vt != NONE) {
        if(input[i] != ' ') { //  value option 이후 다른 option 발견
          return FALSE;
        }
      }
    } //  for - end
  } //  else - end

  if(sf == NONE || st == NONE || ef == NONE || et == NONE 
      || vf == NONE || vt == NONE || !comma1 || !comma2) 
    return FALSE;

  if(st - sf + 1 < 0 || st - sf + 1 > 5)   //  start address 길이 확인
    return FALSE;

  if(et - ef + 1 < 0 || et - ef + 1 > 5)   //  end address 길이 확인
    return FALSE;
  
  if(vt - vf + 1 < 0 || vt - vf + 1 > 2)   //  value address 길이 확인
    return FALSE;

  for(i = sf; i <= st; i++)
    opt1[i - sf] = input[i];
  opt1[i] = '\0';

  for(i = ef; i <= et; i++)
    opt2[i - ef] = input[i];
  opt2[i] = '\0';

  for(i = vf; i <= vt; i++)
    opt3[i - vf] = input[i];
  opt3[i] = '\0';

  *start = 0;
  *end = 0;
  *value = 0;
  
  hex = 1;
  for(i = st - sf; i >= 0; i--) {
    if('0' <= opt1[i] && opt1[i] <= '9')
      *start += (hex * (opt1[i] - '0'));
    else if('A' <= opt1[i] && opt1[i] <= 'F')
      *start += (hex * (opt1[i] - 'A' + 10));
    else if('a' <= opt1[i] && opt1[i] <= 'f')
      *start += (hex * (opt1[i] - 'a' + 10));
    else   //  잘못된 start
      return FALSE;
    hex *= 16;
  }

  hex = 1;
  for(i = et - ef; i >= 0; i--) {
    if('0' <= opt2[i] && opt2[i] <= '9')
      *end += (hex * (opt2[i] - '0'));
    else if('A' <= opt2[i] && opt2[i] <= 'F')
      *end += (hex * (opt2[i] - 'A' + 10));
    else if('a' <= opt2[i] && opt2[i] <= 'f')
      *end += (hex * (opt2[i] - 'a' + 10));
    else   //  잘못된 end 
      return FALSE;
    hex *= 16;
  }

  hex = 1;
  for(i = vt - vf; i >= 0; i--) {
    if('0' <= opt3[i] && opt3[i] <= '9')
      *value += (hex * (opt3[i] - '0'));
    else if('A' <= opt3[i] && opt3[i] <= 'F')
      *value += (hex * (opt3[i] - 'A' + 10));
    else if('a' <= opt3[i] && opt3[i] <= 'f')
      *value += (hex * (opt3[i] - 'a' + 10));
    else   //  잘못된 value
      return FALSE;
    hex *= 16;
  }

  if(*start > *end) 
    return FALSE; 

  return TRUE;
}

int process_command(char* cmd, char* input) { //  qu[it] 명령 수행 시 FALSE 반환(프로그램 종료)
  DIR* dp = NULL;             //  dirent.h
  struct dirent* dir_entry;   //  dirent.h
  struct stat dir_stat;       //  sys/stat.h
  char opt1[MAX_OPT] = {0, }, opt2[MAX_OPT] = {0, }, opt3[MAX_OPT] = {0, };
  char mnemonic[MNEMONIC] = {0, }, opcode[OPCODE] = {0, };
  char queue_input[INPUT_LEN] = {0, };  //  history queue에 삽입될 정제된 명령어
 
  //  q[uit]
  if(!strcmp(cmd, "quit") || !strcmp(cmd, "q")) {
    if(!strcmp(cmd, "quit")) {
      if(!check_no_opt(input, 4)) {
        printf("유효하지 않은 q[uit] 명령\n");
        return TRUE;
      }
    } 
    else {
      if(!check_no_opt(input, 1)) {
        printf("유효하지 않은 q[uit] 명령\n");
        return TRUE;
      }
    }
    delete_queue();
    delete_optable();
    return FALSE;
  }
 
  //  h[elp]
  if(!strcmp(cmd, "help") || !strcmp(cmd, "h")) {
    
    if(!strcmp(cmd, "help")) {
      if(!check_no_opt(input, 4)) {
        printf("유효하지 않은 h[elp] 명령\n");
        return TRUE;
      }
    } 
    else {
      if(!check_no_opt(input, 1)) {
        printf("유효하지 않은 h[elp] 명령\n");
        return TRUE;
      }
    }

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
    
    enqueue(cmd);
  }

  //  d[ir]
  else if(!strcmp(cmd, "dir") || !strcmp(cmd, "d")) {
    
    if(!strcmp(cmd, "dir")) {
      if(!check_no_opt(input, 3)) {
        printf("유효하지 않은 d[ir] 명령\n");
        return TRUE;
      }
    } 
    else {
      if(!check_no_opt(input, 1)) {
        printf("유효하지 않은 d[ir] 명령\n");
        return TRUE;
      }
    }
    
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

    closedir(dp);
    enqueue(cmd);
  }

  //  hi[story]
  else if(!strcmp(cmd, "history") || !strcmp(cmd, "hi")) {
    
    if(!strcmp(cmd, "history")) {
      if(!check_no_opt(input, 7)) {
        printf("유효하지 않은 hi[story] 명령\n");
        return TRUE;
      }
    } 
    else {
      if(!check_no_opt(input, 2)) {
        printf("유효하지 않은 hi[story] 명령\n");
        return TRUE;
      }
    }

    enqueue(cmd);
    print_queue();
  }

  //  reset
  else if(!strcmp(cmd, "reset")) {
    if(!check_no_opt(input, 5)) {
      printf("유효하지 않은 reset 명령\n");
      return TRUE;
    }
    reset_memory();
    enqueue(input);
  }

  //  opcodelist
  else if(!strcmp(cmd, "opcodelist")) {
    
     if(!check_no_opt(input, 10)) {
        printf("유효하지 않은 opcodelist 명령\n");
        return TRUE;
     }

     print_optable();
     enqueue(cmd);
  }
  
  //  option 필요한 명령어
  //  du[mp]
  else if(!strcmp(cmd, "dump") || !strcmp(cmd, "du")) {
    int start, end;

    if(!check_dump(input, cmd, &start, &end, opt1, opt2)) {
      printf("유효하지 않은 du[mp] 명령\n");
      return TRUE;
    }

    if(strlen(opt1) == 0 && strlen(opt2) == 0) {  //  start, end option 없음
      sprintf(queue_input, "%s", cmd);
    }
    else if(strlen(opt1) > 0 && strlen(opt2) == 0) {  //  start option만 존재
      sprintf(queue_input, "%s %s", cmd, opt1);
    }
    else {  //  start, end option 모두 존재
      sprintf(queue_input, "%s %s, %s", cmd, opt1, opt2);
    }

    dump_memory(start, end);
    enqueue(queue_input);
  }
  //  e[dit]
  else if(!strcmp(cmd, "edit") || !strcmp(cmd, "e")) {
    int addr, val;

    if(!check_edit(input, cmd, &addr, &val, opt1, opt2)) {
      printf("유효하지 않은 e[dit] 명령\n");
      return TRUE;  
    }

    edit_memory(addr, val);
    sprintf(queue_input, "%s %s, %s", cmd, opt1, opt2);
    enqueue(queue_input);
  }
  //  f[ill]
  else if(!strcmp(cmd, "fill") || !strcmp(cmd, "f")) {
    int start, end, value;

    if(!check_fill(input, cmd, &start, &end, &value, opt1, opt2, opt3)) {
      printf("유효하지 않은 f[ill] 명령\n");
      return TRUE;
    } 

    fill_memory(start, end, value);
    sprintf(queue_input, "%s %s, %s, %s", cmd, opt1, opt2, opt3);
    enqueue(queue_input);
  }
  //  opcode
  else if(!strcmp(cmd, "opcode")) {
    if(!check_opcode(input, mnemonic, opcode)) {
      printf("유효하지 않은 opcode 명령\n");
      return TRUE;
    }
    sprintf(queue_input, "%s %s", cmd, mnemonic);
    enqueue(queue_input);
  }

  return TRUE;
}
