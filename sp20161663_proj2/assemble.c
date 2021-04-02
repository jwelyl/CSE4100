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

int hex_to_bin(char hex, char* bin) { //  0 ~ F 16진수를 4bits 2진수로 변환
  int dec, i;

  if('0' <= hex && hex <= '9') dec = hex - '0';
  else if('A' <= hex && hex <= 'F') dec = hex - 'A' + 10;
  else {
    printf("opcode error at line %d\n", line);
    return FALSE;
  }

//  printf("dec = %d\n", dec);

  for(i = 3; i >= 0; i--) {
    if(dec % 2 == 0) bin[i] = '0';
    else bin[i] = '1';
    dec = dec / 2;
  }

  //
  //  printf("변환된 이진수 : %s(%zu)\n", bin, strlen(bin));
  //
  return TRUE;
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
    
    if(('0' <= input[0] && input[0] <= '9') || ('A' <= input[0] && input[0] <= 'F')) { 
      //  LOCCTR이 존재하는 줄일 경우 계산하여 pass_2에 전달함
      for(i = 3; i >= 0; i--) {
        if('0' <= input[i] && input[i] <= '9')
          *locctr += (input[i] - '0') * mult;
        else if('A' <= input[i] && input[i] <= 'F')
          *locctr += (input[i] - 'A' + 10) * mult;
        else {
//          printf("{%c}\n", input[i]);
          printf("intermediate file LOCCTR error at line %d\n", line);
          return FALSE;
        }
        mult *= 16;
      }
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

int find_register(char* opr) {
  int ret = NONE;

  if(!strcmp(opr, "A"))        ret = A;
  else if(!strcmp(opr, "X"))   ret = X;
  else if(!strcmp(opr, "L"))   ret = L;
  else if(!strcmp(opr, "B"))   ret = B;
  else if(!strcmp(opr, "S"))   ret = S;
  else if(!strcmp(opr, "T"))   ret = T;
  else if(!strcmp(opr, "F"))   ret = F;
  else if(!strcmp(opr, "PC"))  ret = PC;
  else if(!strcmp(opr, "SW"))  ret = SW;

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

//  printf("intermediate file name : %s\n", mid_filename);

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

    if(!strcmp(mnemonic, "END"))  //  마지막 줄일 경우
      break;
    
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
  int i, j, f;
  //  여기까지는 pass_1과 같음
  //  아래는 pass_2에 추가된 변수
  char h_record[HEADER] = {0, };
  char t_record[TEXT] = {0, };
  char m_record[MODIFICATION] = {0, };
  char e_record[END] = {0, };
  char h_temp[7];  //  Header record 작성용 배열

  char opr1[STRING_SIZE] = {0, }, opr2[STRING_SIZE] = {0, };  //  operand 최대 2개 가능
  char opr[STRING_SIZE] = {0, };  //  symtable에서 찾을 operand('#', '@' 제거)/
  int os1, oe1, os2, oe2, comma;  //  각 operand의 시작, 끝 인덱스, comma 존재
  char opcode[OPCODE] = {0, };  //  optable로부터 opcode를 받아옴
  char obj_code[OBJ_HEX] = {0, }; //  lst, obj 파일에 실제 입력할 hex object code
  int opr_const;  //  operand가 상수인지 확인
  int* pos_to_mod = (int*)malloc(sizeof(int) * format_4);
  //  수정해야 할 4형식 명령어 object code 위치 저장할 배열
  int locctr;         //  현재 줄의 LOCCTR
  int pc;             //  현재 줄의 PROGRAM COUNTER;
  int base = NONE;    //  B 레지스터에 저장된 값
  int disp;           //  object code 작성에 필요한 disp(3형식) 또는 address(4형식)
  int text_start = 0; //  text record가 들어가기 시작할 위치
  int m_idx = 0;

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

  sprintf(t_record, "%7X", locctr);
  t_record[0] = 'T';
  for(i = 0; i < 7; i++) {
    if(t_record[i] == ' ')
      t_record[i] = '0';
  }
  
  //  reading from second line
  while(fgets(input, INPUT_LEN, *fp_mid)) {
    input[strlen(input) - 1] = '\0';  //  '\n' 키 제거
    line += 5;
    memset(obj_code, 0, sizeof(obj_code));  //  obj_code 초기화

    if(!process_input_string(input, 2, &locctr, &ls, &le, &ms, &me, &os, &oe))
      return FALSE;

    if(!strcmp(mnemonic, "END")) // 마지막 줄일 경우
      break;

    if(input[0] == ' ' || input[0] == '\t' || input[0] == '.') { //  주석 또는 빈 줄인 경우
      fprintf(*fp_lst, "%3d\t%s\n", line, input);
      continue;
    }

    if(os != NONE && oe != NONE) {  //  operand 존재할 경우 operand 처리        
      os1 = 0, oe1 = NONE, os2 = NONE, oe2 = NONE, comma = FALSE; //  index 및 comma 초기화
      //  opr1, opr2 초기화
      for(i = 0; i < STRING_SIZE; i++) {
        opr1[i] = 0;
        opr2[i] = 0;
      }
      
      for(i = 0; i < strlen(operand); i++) {
        if(operand[i] == ',') {
          comma = TRUE;
          break;
        }
      }
      
      if(comma) { //  operand가 2개일 경우
        for(i = os1; i < strlen(operand); i++) {
          if(operand[i] == ',' || operand[i] == ' ' || operand[i] == '\t') {
            oe1 = i - 1;
            break;
          }
        }

        oe2 = strlen(operand) - 1;
        for(i = oe2; i >= 0; i--) {
          if(operand[i] == ',' || operand[i] == ' ' || operand[i] == '\t') {
            os2 = i + 1;
            break;
          }
        }
      }
      else oe1 = strlen(operand) - 1;

      for(i = os1; i <= oe1; i++) 
        opr1[i - os1] = operand[i];   //  operand가 1개일 경우, 해당 operand
      if(os2 != NONE && oe2 != NONE)
        for(i = os2; i <= oe2; i++)
          opr2[i - os2] = operand[i];
    } //  operand 분리 end

    if(mnemonic[0] == '+') {  //  4형식일 경우
      for(i = 1; i < strlen(mnemonic); i++)
        mnemonic4[i - 1] = mnemonic[i];
      mnemonic4[i - 1] = '\0';
      f = find_format(mnemonic4);
      if(f != 3) {  //  3, 4형식 opcode mnemonic이 아닐 경우
        printf("Cannot be Format 4 in line %d\n", line);
        return FALSE; 
      }
      f = 4;
      find_opcode(mnemonic4, opcode);
    }
    else {  //  4형식이 아닐 경우
      f = find_format(mnemonic);
      find_opcode(mnemonic, opcode);
    }
    pc = locctr + f;  //  Program Counter

    if(f != NONE) { //  mnemonic이 Assembler Directives가 아닐 경우
      obj_code[0] = opcode[0];
      
      if(f == 1 || f == 2) {
        obj_code[1] = opcode[1];
        
        if(f == 2) {
          int reg1 = find_register(opr1);
          int reg2 = find_register(opr2);

          if(reg1 == NONE) {
            printf("Register error at line %d\n", line);
            return FALSE;
          }
          if(reg2 == NONE) reg2 = 0;
          obj_code[2] = reg1 + '0';
          obj_code[3] = reg2 + '0';
        }
      } //  format 1 or 2 end

      else if(f == 3 || f == 4) {
        int num = 0;
        memset(opr, 0, strlen(opr));    // opr 초기화
        if(os == NONE && oe == NONE) {  //  operand 없을 경우
          //  n-bit : 1, i-bit: 1
          num = 3;  //  11b
        } 
        else if(operand[0] == '#') { //  immediate addressing
          // n-bit : 0, i-bit : 1
          num = 1;  //  01b
          opr_const = TRUE;
          for(i = 1; i < strlen(opr1); i++) {
            if(!('0' <= opr1[i] && opr1[i] <= '9')) opr_const =FALSE;  //  10진수여야 상수임
            opr[i - 1] = opr1[i]; 
          }
        }
        else if(operand[0] == '@') {  //  indirect addressing
          //  n-bit : 1, i-bit : 0
          num = 2;  //  10b
          opr_const = FALSE;
          for(i = 1; i < strlen(opr1); i++)
            opr[i - 1] = opr1[i];
        }
        else {  //  simple addressing
          //  n-bit : 1, i-bit : 1
          num = 3;  //  11b
          opr_const = FALSE;
          for(i = 0; i < strlen(opr1); i++)
            opr[i] = opr1[i];
        }

        if(opcode[1] == '0' || opcode[1] == '4' || opcode[1] == '8')  //  00__b or 01__b or 10__b
          num = (opcode[1] - '0') + num;
        else if(opcode[1] == 'C')         //  11__b
          num = (opcode[1] - 'A' + 10) + num;
        else {
          printf("opcode error(not '0', '4', '8', 'C') at line %d\n", line);
        }

        if(0 <= num && num <= 9)
          obj_code[1] = num + '0';
        else if(10 <= num && num <= 15)
          obj_code[1] = num - 10 + 'A';

        num = 0;
        if(!strcmp(opr2, "X")) {  //  index addressing
          //  x-bit : 1 
          num = 8;  //  1___b
        }

        if(!strcmp(mnemonic, "LDB") || !strcmp(mnemonic4, "LDB")) {
          if(operand[0] == '#') {
            find_locctr(opr, &base); 
          }

          //
          printf("base = %d(%X) at line %d\n", base, base, line);
          //  
        }

        int TA;
        char hex_temp[9] = {0, }; //  int 32-bits, 8-half-bytes
        
        if(os != NONE && oe != NONE) {
          if(!opr_const) {
//            printf("line %d에서 opr %s은 상수 아님!\n", line, opr);

//            printf("TA 찾기 위한 operand : %s at line %d\n", opr, line);
            find_locctr(opr, &TA); //  상수가 아닐경우
          }
          else {  //  상수일 경우
            printf("line %d에서 opr %s은 상수임!\n", line, opr);
            TA = 0;
            int mul = 1;
            for(i = strlen(opr) - 1; i >= 0; i--) {
              printf("%c\n", opr[i]);
              TA += ((opr[i] - '0') * mul);
              mul *= 10;
            }
            printf("line %d에서 opr이 상수일때 TA = %X(%d)\n", line, TA, TA); 
          }
        }
        else TA = 0;

        if(f == 3) {  //  특히 format-3일 경우
          if(os == NONE && oe == NONE)  //  operand가 없을 경우
            disp = 0;
          else if(opr_const) {  //  operand가 상수일 경우
            //  b-bit : 0, p-bit : 0, e-bit : 0
            num += 0; //  _000b 추가
            disp = TA;
          }
          else if(-2048 <= (TA - pc) && (TA -pc) <= 2047) {
            disp = TA - pc; //  PC-relative addressing
            //  b-bit : 0, p-bit : 1, e-bit : 0
            num += 2; //  _010b 추가 
          }
          else if(base == NONE) { //  PC-relative addressing이 불가능한데 Base에 저장된 값이 없을 경우
            printf("Base-relativie addressing error at line %d\n", line);
            return FALSE;
          }
          else if(0 <= (TA - base) && (TA - base) <= 4095) {
            disp = TA - base; //  Base-relative addressing
            //  b-bit : 1, p-bit : 0, e-bit : 0
            num += 4; //  _1__b 추가
          }
          else {  //  Format 3 error
            printf("Invalid Format 3 error(Format 4 needed) at line %d\n", line);
            return FALSE;
          }
        }
        else {  //  format-4일 경우
          disp = TA;  //  direct-addressing
          //  b-bit : 0, p-bit : 0, e-bit : 0
          num += 1; //  ___1b 추가
        }
       
        if(10 <= num && num <= 15)
          obj_code[2] = num - 10 + 'A';
        else if(0 <= num && num <= 9)
          obj_code[2] = num + '0';
        else {
          printf("disp error at line %d\n", line);
          return FALSE;
        }

        sprintf(hex_temp, "%X", disp);  //  disp(또는 address를 16진수로 임시 배열에 옮김)

        int end;
        if(f == 3) end = 5; //  3형식일때 마지막 index
        if(f == 4) end = 7; //  4형식일떼

        if(disp < 0) {  //  3형식일 경우만 해당
          if(f == 4) {
            printf("Cannot be Format 4 at line %d\n", line);
            return FALSE;
          }
          for(i = 7; i >= 5; i--)
            obj_code[i - 2] = hex_temp[i];
        }
        else {
          for(i = 3; i <= end; i++)
            obj_code[i] = '0';
          if(disp > 0) {
            for(i = 0; i < strlen(hex_temp); i++) {
              obj_code[end - strlen(hex_temp) + i + 1] = hex_temp[i];
            }
          } 
        }
          
      } //  format-3,4 end
    } //  f != NONE end

    else {  //  mnemonic이 Assembler Directives일 경우
      if(!strcmp(mnemonic, "WORD"));
      else if(!strcmp(mnemonic, "BYTE")) {
        if(!(os != NONE && oe != NONE)) {
          printf("operand of BYTE directive does not exist at line %d\n", line);
          return FALSE;
        }
        else {
          if(operand[0] == 'C') {
            char hex_temp[3] = {0, };
            int comma = FALSE;
            for(i = 1; ; i++) {
              if(!comma) {
                if(operand[i] == '\'') comma = TRUE;  //  첫 comma일 경우
              }
              else {
                if(operand[i] == '\'') break; //  마지막 comma일 경우   
                memset(hex_temp, 0, 3);
                sprintf(hex_temp, "%X", operand[i]);
                strcat(obj_code, hex_temp);
              }
            }
          }
          else if(operand[0] == 'X') {
            int comma = FALSE;
            int j = 0;
            for(i = 1; ; i++) {
              if(!comma) {
                if(operand[i] == '\'') comma = TRUE;
              }
              else {
                if(operand[i] == '\'') break;
                obj_code[j++] = operand[i];
              }
            }
          }
        }
      } //  BYTE-END
      else ;
    } //  f == NONE end

    if(os != NONE && oe != NONE) {
      if(strlen(operand) == 1) fprintf(*fp_lst, "%3d\t%s\t\t%s\n", line, input, obj_code);
      else fprintf(*fp_lst, "%3d\t%s\t%s\n", line, input, obj_code);
    }
    else
      fprintf(*fp_lst, "%3d\t%s\t\t%s\n", line, input, obj_code);
    
    //  modification 필요한 위치 찾기 
    if(mnemonic[0] == '+' && operand[0] != '#') {
      pos_to_mod[m_idx++] = text_start / 2 + 1;
      printf("line %d에 수정 필요 위치 : %X\n", line, pos_to_mod[m_idx - 1]); 
    }
    text_start += strlen(obj_code);

  } //  while-end

  //  마지막 줄 lst 파일에 쓰기 
  fprintf(*fp_lst, "%3d\t%s\n", line, input);
 
  //  Modification record 작성
  for(i = 0; i < format_4; i++) {
    memset(m_record, 0, 10);
    sprintf(m_record, "%7X", pos_to_mod[i]);
    m_record[0] = 'M';
    for(j = 0; j < 9; j++) {
      if(m_record[j] == ' ')
        m_record[j] = '0';
    }
    strcat(m_record, "05"); //  format-4 address 수정은 5자리
    fprintf(*fp_obj, "%s\n", m_record);
  } 
  //  End record 작성
  int excutable;
  char temp[7] = {0, };
  
  e_record[0] = 'E';
  find_locctr(operand, &excutable);
  printf("excutable : %X\n", excutable);
  dec_to_hex(excutable, temp, 7);
  strcat(e_record, temp);
  fprintf(*fp_obj, "%s\n", e_record);

  line = 0;
  free(pos_to_mod);
  pos_to_mod = NULL;

  return TRUE;
}
