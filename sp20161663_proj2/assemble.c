#include "assemble.h"
#include "memory.h"
#include "optable.h"
#include "symtable.h"

int line;             //  listing file에 입력할 줄 번호(5부터 5씩 증가)
int start_address;    //  starting address
int LOCCTR;           //  Location Counter
int program_size;     //  Program Size = Location Counter - starting address
int format_4;         //  format 4 mnemonic 개수

char program_name[PROGRAM_NAME];
char label[STRING_SIZE];
char mnemonic[STRING_SIZE];
char operand[STRING_SIZE];
char locctr_array[LOCCTR_SIZE];

void init_variables() {
  //  pass1 시작 전 변수를 초기화한다.
  line = 0;
  start_address = 0;
  LOCCTR = 0;
  program_size = 0;
  format_4 = 0;
}

int operand_to_dec() {  //  필요할 경우 10진수 배열을 10진수로 정수로 변환
  int i = strlen(operand) - 1;
  int mult = 1;
  int ret = 0;

  for(; i >= 0; i--) {
    if(operand[i] < '0' || operand[i] > '9') return NONE;
    ret += ((operand[i] - '0') * mult);
    mult *= 10;
  }
  
  return ret;
}

int hex_to_dec(char* hex, int* dec) {  // 16진수를 10진수로 변경 
  //  16진수를 10진수로 변환 성공 시 TRUE, 실패 시 FALSE 반환
  int i = strlen(hex) - 2;
  int mult = 1;
  
  *dec = 0;
  for(; i >= 0; i--) {
    if('0' <= hex[i] && hex[i] <= '9') 
      *dec += ((hex[i] - '0') * mult);
    else if('A' <= hex[i] && hex[i] <= 'F')
      *dec += ((hex[i] - 'A' + 10) * mult);
    else if('a' <= hex[i] && hex[i] <= 'f')
      *dec += ((hex[i] - 'a' + 10) * mult);
    else {
      printf("유효하지 않은 16진수\n");
      return FALSE;
    }

    mult *= 16;
  }

  return TRUE;
}

void reset_indices(int* ls, int* le, int* ms, int* me, int* os, int* oe) {
  *ls = NONE; *ms = NONE; *os = NONE;
  *le = NONE; *me = NONE; *oe = NONE;
}

//  pass 1,2를 위한 입력 문자열 처리
int process_input_string(char* input, int pass, int* locctr,
  int* ls, int* le, int* ms, int* me, int* os, int* oe) {
  //  assembly source file을 한 줄씩 읽어서 label, opcode_mnemonic, operand를 분리
  //  매개변수 pass가 1일 경우, pass_1에 대한 입력 문자열 처리
  //  매개변수 pass가 2일 경우, pass_2에 대한 입력 문자열 처리
  //  pass_2일 경우 매개변수 포인터 locctr에 현재 줄의 LOCCTR 저장
  //  정상 처리되면 TRUE, 에러 발생시 FALSE 반환함
  int comma = FALSE;    //  operand가 두 개일경우 구분
  int sec_opr = FALSE;  //  second operand  
  int i;
  int start = 0;

  reset_indices(ls, le, ms, me, os, oe);

  if(pass == 1) {
    start = 0;
    if(input[0] == '.') // pass1 과정에서 해당 줄은 주석 또는 빈 줄이므로 더 처리할 게 없음
      return TRUE;
  }
  else if(pass == 2) {
    *locctr = 0;
    int mult = 1;

    for(i = 3; i >= 0; i--) {
      if('0' <= input[i] && input[i] <= '9')
        *locctr += (input[i] - '0') * mult;
      else if('A' <= input[i] && input[i] <= 'F')
        *locctr += (input[i] - 'A' + 10) * mult;
      else {
        printf("intermediate file LOCCTR error at line %d\n");
        return FALSE;
      }
      mult *= 16;
    }

    start = 5;
    if(input[0] == ' ' || input[0] == '\t' || input[0] == '.') // pass2 과정에서 해당 줄은 주석 또는 빈 줄이므로 더 처리할 게 없음
      for(i = 0; i < INPUT_LEN; i++)  //  해당 줄이 정말 주석인지 검사     
        if(input[i] == '.') return TRUE;
  }
  else {
    printf("잘못된 pass = %d 입력\n", pass);
    return FALSE;
  }

  if(input[start] == ' ' || input[start] == '\t') {  //  label이 없는 줄일 경우
    for(i = start; i < INPUT_LEN; i++) {
      if(input[i] == '\0') {
        if(*ms != NONE && *me == NONE)  {//  mnemonic만 있고 문자열이 끝날 경우
          *me = i - 1;
        }
        else if(*os != NONE && *oe == NONE) {  //  operand까지 있을 경우
          if(comma && !sec_opr) { //  comma는 있는데 second operand를 못찾았을 경우
            printf("Assembly source file error at line %d during pass %d\n", line, pass);
            return FALSE;
          }

          else if(!comma || (comma && sec_opr)) { //  단일 operand거나, operand 두 개를 모두 찾았을 경우
            *oe = i - 1;
          }
        }

        break;
      } //  if-'\0' end

      if(*ms == NONE && input[i] != ' ' && input[i] != '\t') {//  find mnemonic start
        *ms = i;
      }

      else if(*ms != NONE && *me == NONE ) {  //  mnemonic end finding
        if(input[i] == ' ' || input[i] == '\t') { //  find mnemonic end
          *me = i - 1;
        }
      }

      else if(*me != NONE && *os == NONE) { //  operand start finding
        if(input[i] != ' ' && input[i] != '\t') { //  find operand start
          *os = i;
        }
      }

      else if(*os != NONE && *oe == NONE) {  //  operand end finding
        if(!comma && (input[i] == ' ' || input[i] == '\t')) {//  find operand end without comma
          *oe = i - 1;
        }

        else if(input[i] == ',') // find ','
          comma = TRUE;

        else if(comma && !sec_opr && (input[i] == ' ' || input[i] == '\t')) // continue to find second operand
          continue;

        else if(comma && !sec_opr && input[i] != ' ' && input[i] != '\t') //  find second operand start
          sec_opr = TRUE;

        else if(sec_opr && (input[i] == ' ' || input[i] == '\t')) { //  find second operand end
          *oe = i - 1;
        }
      }

      else if(*oe != NONE) {
        if(input[i] != ' ' && input[i] != '\t') {
          printf("label 존재 x, input[%d] = %c(%d) ", i, input[i], input[i]);
          printf("Assembly source file error at line %d during pass %d\n", line, pass);
          return FALSE;
        }
      }
    } //  for-end
    
    for(i = *ms; i <= *me; i++) 
      mnemonic[i - (*ms)] = input[i];
    mnemonic[i - (*ms)] = '\0';

    if(*os != NONE && *oe != NONE) {  //  operand도 존재할 경우
      for(i = *os; i <= *oe; i++)
        operand[i - (*os)] = input[i];
      operand[i - (*os)] = '\0';
    } 
   
    return TRUE; 
  } //  label 없는 줄 end

  else {  //  label 있는 줄일 경우
    for(i = start; i < INPUT_LEN; i++) {
      if(input[i] == '\0') {
        if(*ms != NONE && *me == NONE)  {//  mnemonic만 있고 문자열이 끝날 경우
          *me = i - 1;
        }

        else if(*os != NONE && *oe == NONE) {  //  operand까지 있을 경우
          if(comma && !sec_opr) { //  comma는 있는데 second operand를 못찾았을 경우
            printf("Assembly source file error at line %d during pass %d\n", line, pass);
            printf("second operand does not exist at line %d\n", line);
            return FALSE;
          }

          else if(!comma || (comma && sec_opr)) {//  단일 operand거나, operand 두 개를 모두 찾았을 경우
            *oe = i - 1;
          }
        }

        break;
      } //  if-'\0' end

      if(*ls == NONE && input[i] != ' ' && input[i] != '\t')  {//  find label start
        *ls = i;
      }
      else if(*ls != NONE && *le == NONE) { //  finding label end
        if(input[i] == ' ' || input[i] == '\t') {//  find label end
          *le = i - 1;
        }
      }

      else if(*le != NONE && *ms == NONE) { //  finding mnemonic start
        if(input[i] != ' ' && input[i] != '\t') {//  find mnemonic start
          *ms = i;
        }
      }

      else if(*ms != NONE && *me == NONE) { //  finding mnemonic end
        if(input[i] == ' ' || input[i] == '\t') {//  find mnemonic end
          *me = i - 1;
        }
      } 

      else if(*me != NONE && *os == NONE) { //  finding operand start
        if(input[i] != ' ' && input[i] != '\t') {//  find operand start
          *os = i;
        }
      }

      else if(*os != NONE && *oe == NONE) {  //  operand end finding
        if(!comma && (input[i] == ' ' || input[i] == '\t')) {//  find operand end without comma
          *oe = i - 1;
        }

        else if(input[i] == ',') // find ','
          comma = TRUE;

        else if(comma && !sec_opr && (input[i] == ' ' || input[i] == '\t')) // continue to find second operand
          continue;

        else if(comma && !sec_opr && input[i] != ' ' && input[i] != '\t') //  find second operand start
          sec_opr = TRUE;

        else if(sec_opr && (input[i] == ' ' || input[i] == '\t')) {//  find second operand end
          *oe = i - 1;
        }
      }

      else if(*oe != NONE) {
        if(input[i] != ' ' && input[i] != '\t') {
          printf("label 존재 o, input[%d] = %c(%d) ", i, input[i], input[i]);
          printf("Assembly source file error at line %d during pass %d\n", line, pass);
          return FALSE;
        }
      }
    } //  for-end

    for(i = *ls; i <= *le; i++)
      label[i - (*ls)] = input[i];
    label[i - (*ls)] = '\0';
  
    for(i = *ms; i <= *me; i++)
      mnemonic[i - (*ms)] = input[i];
    mnemonic[i - (*ms)] = '\0';
    
    if(*os != NONE && *oe != NONE) {  //  operand도 존재할 경우
      for(i = *os; i <= *oe; i++)
        operand[i - (*os)] = input[i];
      operand[i - (*os)] = '\0';
    } 

    return TRUE;
  } //  label 없는 줄 end

  printf("Something was wrong in process_input_string function\n");
  return FALSE;
}

void change_extension(char* filename, char* target, char* extension) {
  int i;

  for(i = 0; i < strlen(filename) - 4; i++)
    target[i] = filename[i];
  strcat(target, extension);  
}

int get_bytes(char* operand) {  //  assembler directives인 BYTE의 operand bytes 계산 
  //  오류 존재시 NONE 반환
  int ret = 0, i;

  if(strlen(operand) < 4) return NONE;
/*
  if(operand[0] == 'C' || operand[0] == 'X') {
    if(operand[1] != '\'')
      return NONE;
    for(i = 2; i < strlen(operand); i++) {
      if(operand[i] == '\'') break;
      ret++;
    }

    if(operand[0] == 'X') {
      if(ret % 2 == 1) return NONE;
      else ret = ret / 2;
    }
  }
*/
   if(operand[1] != '\'')
      return NONE;

  if(operand[0] == 'C') {
    for(i = 2; i < strlen(operand); i++) {
      if(operand[i] == '\'') break;
      ret++;
    }
  }

  else if(operand[0] == 'X') {
    for(i = 2; i < strlen(operand); i++) {
      if(operand[i] == '\'') break;
      if(!(('0' <= operand[i] && operand[i] <= '9') || ('A' <= operand[i] && operand[i] <= 'F')))
        return NONE;  //  16진수가 아닐 경우
      ret++;
    } 
    if(ret % 2 == 1) return NONE;
    else ret = ret / 2;
  }

  else return NONE;

  return ret;
}

int pass_1(char* filename, char* mid_filename, FILE* fp_asm, FILE** fp_mid) {
  char input[INPUT_LEN];
  char mnemonic4[STRING_SIZE] = {0, };  //  mnemonic이 4형식일 경우 대신 사용
  int ls = NONE, le = NONE; //  label start, end index
  int ms = NONE, me = NONE; //  opcode mnemonic start, end index
  int os = NONE, oe = NONE; //  operand start, end index 
  int add; //  add : LOCCTR addition
  int i, f;   //  looping index, format

  //  전역변수 초기화
  init_variables();

  printf("intermediate file name : %s\n", mid_filename);

  *fp_mid = fopen(mid_filename, "w");
  if(!(*fp_mid)) {
    printf("intermediate file open error!\n");
    return FALSE;
  }

  //  read first input line
  fgets(input, INPUT_LEN, fp_asm);
  input[strlen(input) - 1] = '\0';  //  '\n'키 제거

  if(!process_input_string(input, 1, NULL, &ls, &le, &ms, &me, &os, &oe)) 
      return FALSE;
  line += 5;

  if(!strcmp(mnemonic, "START")) {
    if(!hex_to_dec(operand, &start_address)) {
      printf("starting address error at line %d\n", line);
      return FALSE;
    }
    LOCCTR = start_address;
  } //  if START
  else 
    LOCCTR = 0;

  dec_to_hex(LOCCTR, locctr_array, LOCCTR_SIZE);  //  LOCCTR을 출력하기 위해 16진수 배열로 변환
  fprintf(*fp_mid, "%s\t%s\n", locctr_array, input);

  strcpy(program_name, label);

  //  reading from second line
  while(fgets(input, INPUT_LEN, fp_asm)) {
    input[strlen(input) - 1] = '\0';  //  '\n'키 제거
    line += 5;

    if(!process_input_string(input, 1, NULL, &ls, &le, &ms, &me, &os, &oe)) 
      return FALSE;

    if(!strcmp(mnemonic, "END")) {  //  마지막 줄일 경우
      break;
    }
    
    if(input[0] == '.') { //  주석 또는 빈 줄일 경우
      fprintf(*fp_mid, "\t%s\n", input);
      continue;
    }

    if(ls != NONE && le != NONE) {  //  label이 존재하는 줄일 경우
      if(find_label(label)) { //  이미 존재하는 label이 다시 등장할 경우
        printf("Duplicated Label at line %d.\n", line);
        return FALSE;
      }

      else //  label이 처음 등장할 경우
        push_stnode(label, LOCCTR); //  symtable에 label과 LOCCTR 함께 삽입 
    } //  label end

   if(mnemonic[0] == '+') { //  4형식일 경우
    for(i = 1; i < strlen(mnemonic); i++)
      mnemonic4[i - 1] = mnemonic[i];
    mnemonic4[i - 1] = '\0';
    f = find_format(mnemonic4); 
   }
   else f = find_format(mnemonic);

   if(f != NONE) {  //  mnemonic이 assembler directives가 아닐 경우
      if(f == 1)  //  1형식일 경우
        add = 1;
      else if(f == 2) //  2형식일 경우
        add = 2;
      else {  //  3/4 형식일 경우
        if(mnemonic[0] == '+') {// 4형식일 경우
          add = 4;
          //  relocation 시 수정해야할 개수
          if(operand[0] != '#') format_4 += 1;
        } else {
          add = 3;
        }
      }
   }

   else { //  mnemonic이 assembler directives일 경우
     int bytes;

     if(!strcmp(mnemonic, "WORD"))  //  "WORD"일 경우(for SIC machine)
        add = 3;
     else if(!strcmp(mnemonic, "BYTE")) { // "BYTE"일 경우
        if(!(os != NONE && oe != NONE)) {
          printf("operand of BYTE directive does not exist at line %d.\n", line);
          return FALSE;
        }
        else if((bytes = get_bytes(operand)) == NONE) {
          printf("operand of BYTE directive is invalid at line %d.\n", line);
          return FALSE;
        }
        add = bytes;
     }
     else if(!strcmp(mnemonic, "RESB")) { //  "RESB"일 경우
        if(!(os != NONE && oe != NONE)) {
          printf("operand of RESB directive does not exist at line %d.\n", line);
          return FALSE;
        }

       if((bytes = operand_to_dec()) == NONE) {
          printf("operand of RESB directive is invalid at line %d.\n", line);
          return FALSE;
       }
       add = bytes;
     }

     else if(!strcmp(mnemonic, "RESW")) { //  "RESW"일 경우
        if(!(os != NONE && oe != NONE)) {
          printf("operand of RESW directive does not exist at line %d.\n", line);
          return FALSE;
        }
        add = 3;
     }

     else { //  "BASE"
        fprintf(*fp_mid, "\t%s\n", input);
        continue;
     } 
   }
    dec_to_hex(LOCCTR, locctr_array, LOCCTR_SIZE);  //  LOCCTR을 출력하기 위해 16진수 배열로 변환
    fprintf(*fp_mid, "%s\t%s\n", locctr_array, input);
    
    //reset_indices(&ls, &le, &ms, &me, &os, &oe);
    LOCCTR += add;
  } //  while-end

  //  process last line
  dec_to_hex(LOCCTR, locctr_array, LOCCTR_SIZE);
  fprintf(*fp_mid, "\t%s\n", input);
  
  program_size = LOCCTR - start_address;
//
  printf("program name : %s\nprogram_size : %X\n", program_name, program_size);
  printf("수정해야 할 format 4 개수 : %d\n", format_4);
  //
  line = 0;
  fclose(fp_asm);
  return TRUE;
}

int pass_2(char* filename, char* mid_filename, char* lst_filename, char* obj_filename, 
          FILE** fp_mid, FILE** fp_lst, FILE** fp_obj) {
  char input[INPUT_LEN];
  char mnemonic4[STRING_SIZE] = {0, };
  int ls = NONE, le = NONE;
  int ms = NONE, me = NONE;
  int os = NONE, oe = NONE;
  int add;
  int i, f;
  //  여기까지는 pass_1과 같음
  //  아래는 pass_2에 추가된 변수
  char h_record[HEADER] = {0, };
  char t_record[TEXT] = {0, };
  char m_record[MODIFICATION] = {0, };
  char h_temp[7];  //  Header record 작성용 배열

  char obj_code[OBJ_HEX] = {0, }; //  lst, obj 파일에 실제 입력할 hex object code
  char obj_bin[13] = {0, };       //  opcode(0~5), n(6), i(7), x(8), b(9), p(10), e(11)
  //  opcode와 n, i, x, b, p, e 결정 후 obj_code의 앞의 3자리 16진수로 변환
  int* pos_to_mod = (int*)malloc(sizeof(int) * format_4);
  //  수정해야 할 4형식 명령어 object code 위치 저장할 배열
  int locctr; //  현재 줄의 LOCCTR
  int pc;     //  현재 줄의 PROCESS COUNTER;
  int base;   //  B 레지스터에 저장된 값

  *fp_mid = fopen(mid_filename, "r");
  if(!(*fp_mid)) {
    printf("intermediate file open error!\n");
    return FALSE;
  }

  change_extension(filename, lst_filename, ".lst");
  change_extension(filename, obj_filename, ".obj");
  *fp_lst = fopen(lst_filename, "w");
  *fp_obj = fopen(obj_filename, "w");

  //  read first input line
  fgets(input, INPUT_LEN, *fp_mid);
  input[strlen(input) - 1] = '\0';  //  '\n'키 제거

  if(!process_input_string(input, 2, &locctr, &ls, &le, &ms, &me, &os, &oe))
    return FALSE;
  line += 5;

  if(!strcmp(mnemonic, "START"))  //  lst 파일 작성
    fprintf(*fp_lst, "%3d\t%s\n", line, input);
  printf("첫째줄 LOCCTR = %#X\n", locctr);

  //  obj 파일 작성
  h_record[0] = 'H';
  strcat(h_record, program_name);
  for(i = strlen(h_record); i < 7; i++)
    h_record[i] = ' ';

  dec_to_hex(start_address, h_temp, 7);
  strcat(h_record, h_temp); //  시작 주소를 16진수로 변환 후 붙여넣음
  
  dec_to_hex(program_size, h_temp, 7);
  strcat(h_record, h_temp); //  프로그램 크기를 16진수로 변환 후 붙여 넣음
  fprintf(*fp_obj, "%s\n", h_record);
/*
  while(fgets(input, INPUT_LEN, *fp_mid)) {
    input[strlen(input) - 1] = '\0';

    if(!process_input_string(input, 2, &ls, &le, &ms, &me, &os, &oe)) 
      return FALSE;

    if(ls != NONE && le != NONE) 
      printf("label : %s\t", label);
    else printf("\t");

    if(ms != NONE && me != NONE)
      printf("mnemonic : %s\t", mnemonic);
    else printf("\t");

    if(os != NONE && oe != NONE)
      printf("operand : %s\t\n", operand);
    else printf("\t\n");
  }
*/  
  line = 0;
  return TRUE;
}
