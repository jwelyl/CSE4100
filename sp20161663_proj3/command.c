#include "command.h"
#include "memory.h"
#include "queue.h"
#include "optable.h"
#include "symtable.h"
#include "assemble.h"
#include "estab.h"
#include "link_loader.h"

int address = 0;
int assembled = FALSE;         // 가장 최근 assemble 명령이 성공 시 TRUE, 실패 시 FALSE
int latest_assembled = FALSE;  // 최초로 assemble 명령이 성공 시 TRUE로 변경 후 유지, 한번도 성공 못했을 경우 FALSE 

void clear_input_buffer() {
  while(getchar() != '\n');
}

int invalid_command(char* input, char* cmd, int* opt_start) {
  int i;
  int input_over = TRUE;
  int cmd_start = NONE, cmd_end = NONE; //  input 문자열에서 명령어 부분 시작, 끝 인덱스
  for(i = 0; i < INPUT_LEN; i++) {  //  입력받은 문장에서 '\n' 제거
    if(input[i] == '\n') {
      input[i] = '\0';
      input_over = FALSE;
      break;
    }
  } 

  if(input_over) clear_input_buffer();
  
  i = 0;
  while(TRUE) { //  입력 문장에서 명령어 부분만 추출
    if(input[i] == '\0') {
      if(cmd_start == NONE && cmd_end == NONE)  //  명령어가 없음
        return TRUE;
      else if(cmd_start != NONE && cmd_end == NONE)
        cmd_end = i - 1;

      break;
    }

    if(cmd_start == NONE && (input[i] != ' ' && input[i] != '\t')) 
       cmd_start = i; 
    
    else if(cmd_start != NONE && cmd_end == NONE && (input[i] == ' ' || input[i] == '\t')) { 
      cmd_end = i - 1;
      break;
    }

    i++;
  }

  for(i = cmd_start; i <= cmd_end; i++)
    cmd[i - cmd_start] = input[i];
  cmd[i - cmd_start] = '\0';
  *opt_start = i;

  if(!strcmp(cmd, "help") || !strcmp(cmd, "h") || !strcmp(cmd, "dir") || !strcmp(cmd, "d") || 
     !strcmp(cmd, "quit") || !strcmp(cmd, "q") || !strcmp(cmd, "history") || !strcmp(cmd, "hi") || 
     !strcmp(cmd, "dump") || !strcmp(cmd, "du") || !strcmp(cmd, "edit") || !strcmp(cmd, "e") || 
     !strcmp(cmd, "fill") || !strcmp(cmd, "f") || !strcmp(cmd, "reset") || !strcmp(cmd, "opcode") || 
     !strcmp(cmd, "opcodelist") || !strcmp(cmd, "assemble") || !strcmp(cmd, "type") || !strcmp(cmd, "symbol") ||
     !strcmp(cmd, "progaddr") || !strcmp(cmd, "loader") || !strcmp(cmd, "bp") || !strcmp(cmd, "run")) {
      
     return FALSE;
  }
  else {  //  존재하지 않는 명령어 또는 띄어쓰기 없이 바로 option 입력
    //
 //   printf("%s(%zu)\n", cmd, strlen(cmd));
    //
    printf("Invalid command\n");
    return TRUE;
  }
}

/* 
 * h[elp], d[ir], q[uit], hi[story], reset, opcodelist, symbol, run
 * 명령어만 입력받았는지 확인
 * 불필요한 옵션이 있을 경우 FALSE 반환
 */
int check_no_opt(char* input, int opt_start) {
  if(input[opt_start] == '\0') return TRUE;
  
  int i = opt_start;
  for(; i < INPUT_LEN; i++) {
    if(input[i] == '\0') break;
    else if(input[i] == ' ' || input[i] == '\t') continue;
    else return FALSE;
  }

  return TRUE;
}

/*
 * du[mp], e[dit], f[ill], opcode
 * 명령어 다음 option 확인
 */

//  du[mp] [start, end]에서 start, end 결정
int check_dump(char* input, int opt_start, int* start, int* end, char* opt1, char* opt2) {
  int i = opt_start;

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

      if(sf == NONE && (input[i] != ' ' && input[i] != '\t')) { //  start option 시작 부분 발견
        sf = i;
        opt1[i - sf] = input[i];
      }
      else if(sf != NONE && st == NONE) { //  start option 끝 부분 찾는 중
        if(input[i] == ' ' || input[i] == '\t' || input[i] == ',') {  //  start option 끝 부분 발견
          if(input[i] == ',') comma = TRUE;

          st = i - 1;
          opt1[i - sf] = '\0';
        }
        else opt1[i - sf] = input[i];
      }
      else if(st != NONE && !comma) { //  start option 찾은 뒤 ',' 찾는 중
        if(input[i] != ',' && input[i] != ' ' && input[i] != '\t') { // ',' 이전에 end option이 나올 경우
          return FALSE;
        }
        else if(input[i] == ',') { // ','를 찾은 경우
          comma = TRUE;
        }
      }
      else if(comma && ef == NONE) {  //  ',' 찾은 뒤 end option 찾는 중
        if(input[i] != ' ' && input[i] != '\t') { //  end option 시작 부분 발견
          ef = i;
          opt2[i - ef] = input[i];
        }
      }
      else if(ef != NONE && et == NONE) { //  end option 끝 부분 찾는 중
        if(input[i] == ' ' || input[i] == '\t') { //  end option 끝 부분 발견
          et = i - 1;
          opt2[i - ef] = '\0';
        }
        else {
          opt2[i - ef] = input[i];
        }
      }
      else if(et != NONE) {
        if(input[i] != ' ' && input[i] != '\t') { //  end option 이후 다른 option 발견
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

      if(*start >= MAX_MEM_SIZE)  //  start가 범위(00000 ~ FFFFF)를 초과할 경우
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
      if(*end >= MAX_MEM_SIZE)  //  end가 범위(00000 ~ FFFFF)를 초과할 경우
        return FALSE;

      if(*end < *start)  //  [start, end] 범위가 잘못됨
        return FALSE;
    } 
  }

  return TRUE;
}

//  opcode 명령 option 찾기
int check_opcode(char* input, int opt_start, char* mnemonic, char* opcode) {
  int i;
  int ms = NONE, me = NONE; //  mnemonic 옵션의 존재 여부 check(mnemonic 시작 index, 끝 index)

  if(input[opt_start] == '\0')   //  mnemonic 없이 명령이 끝날 경우
    return FALSE;
  
  for(i = opt_start + 1; i < INPUT_LEN; i++) {
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
    
    else if((input[i] != ' ' && input[i] != '\t') && (input[i] < 'A' || 'Z' < input[i]))   //  유효하지 않은 mnemonic
        return FALSE;

    else if(ms == NONE && ('A' <= input[i] && input[i] <= 'Z')) { //  mnemonic start index 찾음
        ms = i;
        continue;
    }

    else if(ms != NONE && me == NONE) { //  mnemonic start index를 찾은 상태에서
      if(input[i] == ' ' || input[i] == '\t') {
        me = i - 1;
        continue;
      }
      else if('A' <= input[i] && input[i] <= 'Z') continue;;
    }

    else if(me != NONE) {
      if(input[i] == ' ' || input[i] == '\t') continue;
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

//  assemble, type 명령어 option 찾기
//  두 명령어 모두 같은 option을 요구하므로 하나의 함수로 option을 체크할수 있다.
int check_assemble_or_type(char* input, int opt_start, char* filename) {
  int i;
  int fs = NONE, fe = NONE; //  filename 옵션의 존재 여부 check(filename 시작 index, 끝 index)

  if(input[opt_start] == '\0') // filename 없이 명령이 끝난 경우
    return FALSE;

  for(i = opt_start + 1; i < INPUT_LEN; i++) {
    if(input[i] == '\0') {
      if(fs == NONE && fe == NONE) // filename option을 발견하지 못하고 명령이 끝난 경우
        return FALSE;
      else if(fs != NONE && fe == NONE) { //  filename option 후에 바로 명령이 끝난 경우
        fe = i - 1;
        break;
      }
      else if(fs != NONE && fe != NONE) //  filename option 후에 ' '만 존재하고 명령이 끝난 경우
        break;
    } //  if-'\0' end

    /*
    else if((input[i] != ' ' && input[i] != '\t') && (input[i] < 'A' || 'Z' < input[i]) &&
            (input[i] < 'a' || 'z' < input[i]) && (input[i] < '0' || '9' < input[i]) && input[i] != '.')
      return FALSE; //  유효하지 않은 filename
    
    else if(fs == NONE && (('A' <= input[i] && input[i] <= 'Z') || 
            ('a' <= input[i] && input[i] <= 'z') || ('0' <= input[i] && input[i] <= '9'))) {
      //  filename start indexfmf ckwdma
      fs = i;
      continue;
    }
    */

    else if(fs == NONE && (input[i] != ' ' && input[i] != '\t')) {  //  filename start index 찾음
      fs = i;
      continue;
    }

    else if(fs != NONE && fe == NONE) { //  filename start index를 찾은 상태에서
      if(input[i] == ' ' || input[i] == '\t') {
        fe = i - 1;
        continue;
      }
      else continue;
    }

    else if(fe != NONE) {
      if(input[i] == ' ' || input[i] == '\t') continue;
      else return FALSE;
    }
  } //  for-end

  for(i = fs; i <= fe; i++)
    filename[i - fs] = input[i];
  filename[i] = '\0';
  return TRUE;
}

//  e[dit] address, value에서 address, value 결정
int check_edit(char* input, int opt_start, int* addr, int* val, char* opt1, char* opt2) {
  int i = opt_start;
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

      if(af == NONE && input[i] != ' ' && input[i] != '\t')  //  address option 시작 부분 발견
        af = i;

      else if(af != NONE && at == NONE) {  //  address option 끝 부분 찾는 중
        if(input[i] == ' ' || input[i] == '\t' || input[i] == ',') {  //  address option 끝 부분 발견
          if(input[i] == ',') comma = TRUE;

          at = i - 1;
        }
      }

      else if(at != NONE && !comma) { //  address option 찾은 뒤 ',' 찾는 중
        if(input[i] != ',' && input[i] != ' ' && input[i] != '\t')   //  ',' 이전에 value option이 나올 경우
          return FALSE;
        else if(input[i] == ',')   //  ','를 찾은 경우
          comma = TRUE;
      }
      
      else if(comma && vf == NONE) {  // ',' 찾은 뒤 value option 찾는 중
        if(input[i] != ' ' && input[i] != '\t')  //  value option 시작 부분 발견
          vf = i;
      }

      else if(vf != NONE && vt == NONE) { //  value option 끝 부분 찾는 중
        if(input[i] == ' ' || input[i] == '\t')  //  value option 끝 부분 발견
          vt = i - 1;
      }
      else if(vt != NONE) { 
        if(input[i] != ' ' && input[i] != '\t')  //  value option 이후 다른 option 발견
          return FALSE;
      }
    } //  for end
  } //  else end

  if(af == NONE || at == NONE || vf == NONE || vt == NONE || !comma) 
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

  if(*addr >= MAX_MEM_SIZE) //  address 범위(00000 ~ FFFFF) 초과
    return FALSE;

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

  if(*val >= 256) //  value 범위(00 ~ FF) 초과
    return FALSE;

  return TRUE;
}

//  f[ill] start, end, value에서 start, end, value 결정
int check_fill(char* input, int opt_start, int* start, int* end, int* value,
  char* opt1, char* opt2, char* opt3) {
  int i = opt_start;

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

      if(sf == NONE && input[i] != ' ' && input[i] != '\t')  //  start option 시작 부분 발견
        sf = i;

      else if(sf != NONE && st == NONE) { //  start option 끝 부분 찾는 중
        if(input[i] == ' ' || input[i] == '\t' || input[i] == ',') {  //  start option 끝 부분 발견
          if(input[i] == ',') 
            comma1 = TRUE;

          st = i - 1;
        }
      }

      else if(st != NONE && !comma1) {  //  start option 찾은 뒤 ',' 찾는 중
        if(input[i] != ',' && input[i] != ' ' && input[i] != '\t')   //  첫번째 ',' 이전에 end option이 나올 경우
          return FALSE;  
        else if(input[i] == ',')   //  첫번째 ','를 찾은 경우
          comma1 = TRUE;
      }

      else if(comma1 && ef == NONE) { //  첫번째 ',' 찾은 뒤 end option 찾는 중
        if(input[i] != ' ' && input[i] != '\t')  //  end option 시작 부분 발견
          ef = i;
      }

      else if(ef != NONE && et == NONE) { //  end option 끝 부분 찾는 중
        if(input[i] == ' ' || input[i] == '\t' || input[i] == ',') {  //  end option 끝 부분 발견
          if(input[i] == ',') 
            comma2 = TRUE;

          et = i - 1;
        }
      }

      else if(et != NONE && !comma2) {  //  end option 찾은 뒤 ',' 찾는 중
        if(input[i] != ',' && input[i] != ' ' && input[i] != '\t')   //  두번째 ',' 이전에 value option이 나올 경우
          return FALSE;  
        else if(input[i] == ',')   //  두번째 ','를 찾은 경우
          comma2 = TRUE;
      }

      else if(comma2 && vf == NONE) { //  두 번째 ',' 찾은 뒤 value option 찾는 중
        if(input[i] != ' ' && input[i] != '\t')  //  value option 시작 부분 발견
          vf = i;
      }

      else if(vf != NONE && vt == NONE) { //  value option 끝 부분 찾는 중
        if(input[i] == ' ' || input[i] == '\t')  //  value option 끝 부분 발견
          vt = i - 1;
      }

      else if(vt != NONE) {
        if(input[i] != ' ' || input[i] != '\t') { //  value option 이후 다른 option 발견
          return FALSE;
        }
      }
    } //  for - end
  } //  else - end

  if(sf == NONE || st == NONE || ef == NONE || et == NONE 
      || vf == NONE || vt == NONE || !comma1 || !comma2) 
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

  if(*start >= MAX_MEM_SIZE)  //  start 범위(00000 ~ FFFFF) 초과
    return FALSE;

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

  if(*end >= MAX_MEM_SIZE)  //  end 범위(00000 ~ FFFFF) 초과
    return FALSE;

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

  if(*value >= 256) //  value 범위(00 ~ FF) 초과
    return FALSE;

  if(*start > *end) 
    return FALSE; 

  return TRUE;
}

/* Proj3 progaddr check */
int check_progaddr(char* input, int opt_start, char* progaddr) {
  int i = opt_start;
  int addrs =NONE, addre = NONE;

  if(input[i] == '\0')  //  address 옵션 없이 끝날 경우
    return FALSE;
  
  for(; i < INPUT_LEN; i++) {
    if(input[i] == '\0') {
      if(addrs == NONE && addre == NONE)  //  address 옵션 없이 명령이 끝난 경우
        return FALSE;
      else if(addrs != NONE && addre == NONE) //  address 옵션 후에 명령이 끝난 경우
        addre = i - 1;
      break;
    } //  if '\0' end
    
    else if(addrs == NONE && input[i] != ' ' && input[i] != '\t') { //  address start index를 찾음
      addrs = i;
      continue;
    }

    else if(addrs != NONE && addre == NONE) { //  address start index를 찾은 상태에서
      if(input[i] == ' ' || input[i] == '\t') {
        addre = i - 1;  //  address end index 찾은 경우
        continue;
      }
      else continue;
    }

    else if(addre != NONE) {  //  address index 찾은 경우
      if(input[i] == ' ' || input[i] == '\t') continue;
      else return FALSE;  //  불필요한 index 나타난 경우
    }
  } //  for-end

  for(i = addrs; i <= addre; i++)
    progaddr[i - addrs] = input[i];
  progaddr[i = addrs] = '\0';

  return TRUE;
}

/* Proj3 loader check */
int check_loader(char* input, int opt_start, char* obj_1, char* obj_2, char* obj_3) {
  int i = opt_start;

  int f1s = NONE, f1e = NONE; //  obj_file 1 start, end index
  int f2s = NONE, f2e = NONE; //  obj_file 2 start, end index 
  int f3s = NONE, f3e = NONE; //  obj_file 3 start, end index

  if(input[i] == '\0')  //  obj file 옵션 없이 loader일 경우
    return FALSE;
  else {  //  loader ...
    i++;

    for(; i < INPUT_LEN; i++) {
      if(input[i] == '\0') {  //  입력 문장 끝에서
        if(f1s == NONE) //  obj file이 하나도 없을 경우
          return FALSE;
        else if(f1s != NONE && f1e == NONE) //  obj file이 하나 있을 경우
          f1e = i - 1;
        else if(f1s != NONE && f1e != NONE && f2s != NONE && f2e == NONE) //  obj file이 두 개 있을 경우
          f2e = i - 1;
        else if(f1s != NONE && f1e != NONE && f2s != NONE && f2e != NONE
                && f3s != NONE && f3e == NONE) // obj file이 세 개 있을 경우
          f3e = i - 1;
        else {
          printf("Something was wrong in check_loader function\n");
          return FALSE;
        }

        break;
      } //  if '\0' end

      if(f1s == NONE && input[i] != ' ' && input[i] != '\t') // obj_f1 시작 부분 발견
        f1s = i;

      else if(f1s != NONE && f1e == NONE) { //  obj_f1 끝 부분 찾는 중
        if(input[i] == ' ' || input[i] == '\t') //  obj_f1 끝 부분 발견
          f1e = i - 1;
      }

      else if(f1e != NONE && f2s == NONE) { //  obj_f2 시작 부분 찾는 중
        if(input[i] != ' ' && input[i] != '\t') //  obj_f2 시작 부분 발견
          f2s = i;
      }

      else if(f2s != NONE && f2e == NONE) { //  obj_f2 끝 부분 찾는 중
        if(input[i] == ' ' || input[i] == '\t') //  obj_f2 끝 부분 발견
          f2e = i - 1;
      }

      else if(f2e != NONE && f3s == NONE) { //  obj_f3 시작 부분 찾는 중
        if(input[i] != ' ' && input[i] != '\t') //  obj_f3 시작 부분 발견
          f3s = i;
      }

      else if(f3s != NONE && f3e == NONE) { //  obj_f3 끝 부분 찾는 중
        if(input[i] == ' ' || input[i] == '\t') //  obj_f3 끝 부분 발견
          f3e = i - 1;
      }

      else if(f3e != NONE) { //  3개의 obj file 찾은 후
        if(input[i] != ' ' && input[i] != '\t') { //  불필요한 옵션 존재
          printf("Object file 개수 3개 초과!\n");
          return FALSE;
        }
      }
    } //  for end
  } //  else loader end

  if(f1s == NONE || f1e == NONE) {
    printf("Object file이 하나도 존재하지 않음!\n");
    printf("Something was wrong in check_loader function\n");
    return FALSE;
  }
/*
  printf("f1s = %d, f1e = %d\n", f1s, f1e);
  printf("f2s = %d, f2e = %d\n", f2s, f2e);
  printf("f3s = %d, f3e = %d\n", f3s, f3e);
*/  
  for(i = f1s; i <= f1e; i++)
    obj_1[i - f1s] = input[i];
  obj_1[i - f1s] = '\0';

  if(f2s != NONE && f2e != NONE) {  //  두 번째 obj file이 존재할 경우
    for(i = f2s; i <= f2e; i++)
      obj_2[i - f2s] = input[i];
    obj_2[i - f2s] = '\0';
  }

  if(f3s != NONE && f3e != NONE) {  //  세 번째 obj file이 존재할 경우
    for(i = f3s; i <= f3e; i++)
      obj_3[i - f3s] = input[i];
    obj_3[i -f3s] = '\0';
  }

  return TRUE;
}

int process_command(char* cmd, char* input, int opt_start) { //  qu[it] 명령 수행 시 FALSE 반환(프로그램 종료)
  DIR* dp = NULL;             //  dirent.h
  struct dirent* dir_entry;   //  dirent.h
  struct stat dir_stat;       //  sys/stat.h
  char opt1[MAX_OPT] = {0, }, opt2[MAX_OPT] = {0, }, opt3[MAX_OPT] = {0, };
  char mnemonic[MNEMONIC] = {0, }, opcode[OPCODE] = {0, };
  char filename[FILENAME] = {0, };  //  입력 파일명
  char* mid_filename = "intermediate_file"; //  중간 파일명
  char lst_filename[FILENAME] = {0, };  //  lst 파일명
  char obj_filename[FILENAME] = {0, };  //  obj 파일명
  char queue_input[INPUT_LEN] = {0, };  //  history queue에 삽입될 정제된 명령어
  FILE* fp = NULL;  //  입력 파일 포인터
  FILE* fp_mid = NULL;  //  중간 파일 포인터
  FILE* fp_lst = NULL;  //  lst 파일 포인터
  FILE* fp_obj = NULL;  //  obj 파일 포인터

  char file_read[INPUT_LEN];

  /* Proj3 추가 변수 */
  //  progaddr 명령의 인자인 프로그램 시작 주소
  char prog_addr[SYMBOL_SIZE] = {0, };

  //  loader 명령의 인자인 object file 이름(최대 3개)
  char obj_f1[FILENAME] = {0, };
  char obj_f2[FILENAME] = {0, };
  char obj_f3[FILENAME] = {0, };

  //  입력 obj file 포인터
  FILE* fp_obj1 = NULL;
  FILE* fp_obj2 = NULL;
  FILE* fp_obj3 = NULL;
 
  //  q[uit]
  if(!strcmp(cmd, "quit") || !strcmp(cmd, "q")) {
    if(!check_no_opt(input, opt_start)) {
      printf("유효하지 않은 q[uit] 명령\n");
      return TRUE;
    }
    delete_queue();
    delete_optable();
    delete_latest_symtable();
    return FALSE;
  }
 
  //  h[elp]
  if(!strcmp(cmd, "help") || !strcmp(cmd, "h")) {
    if(!check_no_opt(input, opt_start)) {
      printf("유효하지 않은 h[elp] 명령\n");
      return TRUE;
    }

    printf("h[elp]");
    printf("\nd[ir]");
    printf("\nq[uit]");
    printf("\nhi[story]");
    printf("\ndu[mp] [start, end]");
    printf("\ne[dit] address, value");
    printf("\nf[ill] start, end, value");
    printf("\nreset");
    printf("\nopcode mnemonic");
    printf("\nopcodelist");
    printf("\nassemble filename");
    printf("\ntype filename");
    printf("\nsymbol");
    printf("\nprogaddr [address]");
    printf("\nloader [object filename1] [object filename2] [...]");
    printf("\nbp [address]");
    printf("\nbp clear");
    printf("\nbp");
    printf("\nrun");
    
    enqueue(cmd);
  }

  //  d[ir]
  else if(!strcmp(cmd, "dir") || !strcmp(cmd, "d")) {
    if(!check_no_opt(input, opt_start)) {
      printf("유효하지 않은 d[ir] 명령\n");
      return TRUE;
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
    if(!check_no_opt(input, opt_start)) {
      printf("유효하지 않은 hi[story] 명령\n");
      return TRUE;
    }

    enqueue(cmd);
    print_queue();
  }

  //  reset
  else if(!strcmp(cmd, "reset")) {
    if(!check_no_opt(input, opt_start)) {
      printf("유효하지 않은 reset 명령\n");
      return TRUE;
    }
    reset_memory();
    enqueue(input);
  }

  //  opcodelist
  else if(!strcmp(cmd, "opcodelist")) {
     if(!check_no_opt(input, opt_start)) {
        printf("유효하지 않은 opcodelist 명령\n");
        return TRUE;
     }

     print_optable();
     enqueue(cmd);
  }

  //  symbol
  else if(!strcmp(cmd, "symbol")) {
    if(!check_no_opt(input, opt_start)) {
      printf("유효하지 않은 symbol 명령\n");
      return TRUE;
    }

    if(!latest_assembled) {  //  단 한번도 assemble 명령이 성공한 적이 없을 경우
      printf("assemble 명령이 수행된 적이 없음\n");
    }
    else  //  assemble이 한번이라도 성공했을 경우 LABEL 출력 
      print_symtable();
    
    enqueue(cmd);
  }
  
  //  option 필요한 명령어
  //  du[mp]
  else if(!strcmp(cmd, "dump") || !strcmp(cmd, "du")) {
    int start, end;

    if(!check_dump(input, opt_start, &start, &end, opt1, opt2)) {
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

    if(!check_edit(input, opt_start, &addr, &val, opt1, opt2)) {
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

    if(!check_fill(input, opt_start, &start, &end, &value, opt1, opt2, opt3)) {
      printf("유효하지 않은 f[ill] 명령\n");
      return TRUE;
    } 

    fill_memory(start, end, value);
    sprintf(queue_input, "%s %s, %s, %s", cmd, opt1, opt2, opt3);
    enqueue(queue_input);
  }
  //  opcode
  else if(!strcmp(cmd, "opcode")) {
    if(!check_opcode(input, opt_start, mnemonic, opcode)) {
      printf("유효하지 않은 opcode 명령\n");
      return TRUE;
    }
    sprintf(queue_input, "%s %s", cmd, mnemonic);
    enqueue(queue_input);
  }
  //  assemble
  else if(!strcmp(cmd, "assemble")) {
    int error_flag = FALSE, i;
    char extension[5] = {0};

    assembled = FALSE;

    if(!check_assemble_or_type(input, opt_start, filename)) 
      error_flag = TRUE; 

    //  입력받은 소스 파일명 길이 확인
    if(strlen(filename) < 5) { // 파일명 길이가 확장자 .asm 포함하여 5보다 작을 경우
        printf("유효하지 않은 파일명 %s\n", filename);
        error_flag = TRUE;
    }
    //  입력받은 소스 파일 확장자 확인
    else {
      for(i = strlen(filename) - 4; i < strlen(filename); i++)
        extension[i - strlen(filename) + 4] = filename[i];

      if(strcmp(extension, ".asm")) {
        printf("입력 파일 %s가  assembly source file이 아님.\n", filename);
        error_flag = TRUE;
      }

      else {
        fp = fopen(filename, "r");
        if(!fp) {
          printf("입력 파일 %s가 존재하지 않음.\n", filename);
          error_flag = TRUE;
        }
      }
    }

    if(!error_flag) {

      if(!pass_1(filename, mid_filename, fp, &fp_mid)) {  //  pass 1 과정에서 error 발생
        fclose(fp);
        
        if(!fp_mid)
          fclose(fp_mid);
        
        remove(mid_filename); //  intermediate file 제거
        delete_symtable();    //  남은 symtable 제거
        return TRUE;
      }

      //  pass1 성공 시 intermdiate file 닫음
      fclose(fp_mid);
      
      if(!pass_2(filename, mid_filename, lst_filename, obj_filename, &fp_mid, &fp_lst, &fp_obj)) { //  pass 2 과정에서 error 발생 
        if(!fp_mid)      
          fclose(fp_mid);
        fclose(fp_lst);
        fclose(fp_obj);
        remove(mid_filename);
        remove(lst_filename);
        remove(obj_filename);
        delete_symtable();  //  pass 1에서 만들어진 symtable 제거
        return TRUE;        
      }

      assembled = TRUE;  //  가장 최근의 assemble 명령 성공함
      if(!latest_assembled) {  //  최초로 assemble 명령이 성공할 경우
        latest_assembled = TRUE;
      }
      //  assemble이 성공했으므로 latest_table을 갱신함
      if(!make_latest_symtable()) { //  Label이 하나도 없을 경우
        if(!fp_mid)      
          fclose(fp_mid);
        fclose(fp_lst);
        fclose(fp_obj);
        remove(mid_filename);
        remove(lst_filename);
        remove(obj_filename);
        delete_symtable();
        return TRUE;
      }

      fclose(fp_mid);
      fclose(fp_lst);
      fclose(fp_obj);
      remove(mid_filename);

      printf("[%s], [%s]\n", lst_filename, obj_filename);
      sprintf(queue_input, "%s %s", cmd, filename);
      enqueue(queue_input);
    }
    else {
      printf("유효하지 않은 assemble 명령\n");
      return TRUE;
    }
  }
  //  type
  else if(!strcmp(cmd, "type")) {
    if(!check_assemble_or_type(input, opt_start, filename)) {
      printf("유효하지 않은 type 명령\n");
      return TRUE;
    }

    fp = fopen(filename, "r");
    if(!fp) {
      printf("%s 파일이 존재하지 않음\n", filename);
      return TRUE;
    }

    while(fgets(file_read, INPUT_LEN, fp))
      printf("%s", file_read);

    fclose(fp);
    fp = NULL;
    sprintf(queue_input, "%s %s", cmd, filename);
    enqueue(queue_input);
  }

  /* Proj3 추가 */
  //  progaddr
  else if(!strcmp(cmd, "progaddr")) {
    int i;
    int addr = 0;
    int num = 1;

    if(!check_progaddr(input, opt_start, prog_addr)) {
      printf("유효하지 않은 progaddr 명령\n");
      return TRUE;
    }

    //  int로 변환
    for(i = strlen(prog_addr) - 1; i >= 0; i--) {
      if('0' <= prog_addr[i] && prog_addr[i] <= '9')
        addr += (prog_addr[i] - '0') * num;
      else if('A' <= prog_addr[i] && prog_addr[i] <= 'F')
        addr += (prog_addr[i] - 'A' + 10) * num;
      else if('a' <= prog_addr[i] && prog_addr[i] <= 'f')
        addr += (prog_addr[i] - 'a' + 10) * num;
      else {
        printf("유효하지 않은 progaddr 주소 입력\n");
        return TRUE;
      }

      num *= 16;
    }

    set_progaddr(addr);
    sprintf(queue_input, "%s %s", cmd, prog_addr);
    enqueue(queue_input);
  }

  //  loader
  else if(!strcmp(cmd, "loader")) {
    int i;
    char extension[5] = {0, };

    if(!check_loader(input, opt_start, obj_f1, obj_f2, obj_f3)) {
      printf("유효하지 않은 loader 명령\n");
      return TRUE;
    }
   
    if(strlen(obj_f1) < 5) {
      printf("첫 번째 object file 명이 유효하지 않음.\n");
      return TRUE;
    }

    for(i = strlen(obj_f1) - 4; i < strlen(obj_f1); i++)
      extension[i - strlen(obj_f1) + 4] = obj_f1[i];
    
    if(strcmp(extension, ".obj")) {
      printf("첫 번째 입력 파일 %s가 object file이 아님.\n", obj_f1);
      return TRUE;
    }

    fp_obj1 = fopen(obj_f1, "r");
    if(!fp_obj1) {
      printf("첫 번째 입력 파일 %s가 존재하지 않음.\n", obj_f1);
      return TRUE;
    }

    if(obj_f2[0] != '\0') { //  두 번째 파일 입력이 존재할 경우
      if(strlen(obj_f2) < 5) {
        printf("두 번째 object file 명이 유효하지 않음.\n");
        return TRUE;
      }

      for(i = strlen(obj_f2) - 4; i < strlen(obj_f2); i++)
        extension[i - strlen(obj_f2) + 4] = obj_f2[i];
    
      if(strcmp(extension, ".obj")) {
        printf("두 번째 입력 파일 %s가 object file이 아님.\n", obj_f2);
        return TRUE;
      }

      fp_obj2 = fopen(obj_f2, "r");
      if(!fp_obj2) {
        printf("두 번째 입력 파일 %s가 존재하지 않음.\n", obj_f2);
        return TRUE;
      }
    }

    if(obj_f3[0] != '\0') { //  세 번째 파일 입력이 존재할 경우
      if(strlen(obj_f3) < 5) {
        printf("세 번째 object file 명이 유효하지 않음.\n");
        return TRUE;
      }

      for(i = strlen(obj_f3) - 4; i < strlen(obj_f3); i++)
        extension[i - strlen(obj_f3) + 4] = obj_f3[i];
    
      if(strcmp(extension, ".obj")) {
        printf("세 번째 입력 파일 %s가 object file이 아님.\n", obj_f3);
        return TRUE;
      }

      fp_obj3 = fopen(obj_f3, "r");
      if(!fp_obj3) {
        printf("세 번째 입력 파일 %s가 존재하지 않음.\n", obj_f3);
        return TRUE;
      }
    }

    if(!loader_pass1(fp_obj1, fp_obj2, fp_obj3)) return TRUE; //  pass 1 수행, 실패할 경우 loader 종료
    
    //  pass 1 끝난 후 pass 2를 위해 파일 닫기
    if(fp_obj1) fclose(fp_obj1);
    if(fp_obj2) fclose(fp_obj2);
    if(fp_obj3) fclose(fp_obj3);
    
    //  pass 2를 위한 파일 열기(pass 1 이전에 파일 검사를 했으므로 또 할 필요 없음)
    fp_obj1 = fopen(obj_f1, "r");
    fp_obj2 = fopen(obj_f2, "r");
    fp_obj3 = fopen(obj_f3, "r");

    if(!loader_pass2(fp_obj1, fp_obj2, fp_obj3)) return TRUE;

    //
    printf("loader 명령은 구현 중\n");
    //

    if(fp_obj1) fclose(fp_obj1);
    if(fp_obj2) fclose(fp_obj2);
    if(fp_obj3) fclose(fp_obj3);
    delete_estable();
    sprintf(queue_input, "%s %s %s %s", cmd, obj_f1, obj_f2, obj_f3);
    enqueue(queue_input);
  }

  return TRUE;
}
