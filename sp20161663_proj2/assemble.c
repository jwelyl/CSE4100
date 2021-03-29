#include "assemble.h"
#include "memory.h"
#include "optable.h"
#include "symtable.h"

int line = 0;
int LOCCTR = 0;
char program_name[PROGRAM_NAME];
char label[STRING_SIZE];
char mnemonic[STRING_SIZE];
char operand[STRING_SIZE];

int operand_to_dec() {  //  필요할 경우 10진수 배열을 10진수로 정수로 변환
  int i = strlen(operand) - 2;
  int mult = 1;
  int ret = 0;

  for(; i >= 0; i--) {
    ret += (operand[i] * mult);
    mult *= 10;
  }

  return ret;
}

int hex_to_dec(char* hex, int* dec) {  // 16진수를 10진수로 변경 
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

int process_input_string(char* input,
  int* ls, int* le, int* ms, int* me, int* os, int* oe) {
  //  assembly source file을 한 줄씩 읽어서 label, opcode_mnemonic, operand를 분리
  //  정상 처리되면 TRUE, 에러 발생시 FALSE 반환함
  int comma = FALSE;    //  operand가 두 개일경우 구분
  int sec_opr = FALSE;  //  second operand  
  int i;

  if(input[0] == '.') // 해당 줄은 주석 또는 빈 줄이므로 더 처리할 게 없음
    return TRUE;
  else if(input[0] == ' ' || input[0] == '\t') {  //  label이 없는 줄일 경우
    for(i = 0; i < INPUT_LEN; i++) {
      if(input[i] == '\0') {
        if(*ms != NONE && *me == NONE)  //  mnemonic만 있고 문자열이 끝날 경우
          *me = i - 1;
        else if(*os != NONE && *oe == NONE) {  //  operand까지 있을 경우
          if(comma && !sec_opr) { //  comma는 있는데 second operand를 못찾았을 경우
            printf("Assembly source file error at line %d\n", line);
            return FALSE;
          }

          else if(!comma || (comma && sec_opr)) //  단일 operand거나, operand 두 개를 모두 찾았을 경우
            *oe = i - 1;
        }

        break;
      } //  if-'\0' end

      if(*ms == NONE && input[i] != ' ' && input[i] != '\t') //  find mnemonic start
        *ms = i;

      else if(*ms != NONE && *me == NONE ) {  //  mnemonic end finding
        if(input[i] == ' ' || input[i] == '\t')  //  find mnemonic end
          *me = i - 1;
      }

      else if(*me != NONE && *os == NONE) { //  operand start finding
        if(input[i] != ' ' && input[i] != '\t') //  find operand start
          *os = i;
      }

      else if(*os != NONE && *oe == NONE) {  //  operand end finding
        if(!comma && (input[i] == ' ' || input[i] == '\t')) //  find operand end without comma
          *oe = i - 1;

        else if(input[i] == ',') // find ','
          comma = TRUE;

        else if(comma && !sec_opr && (input[i] == ' ' || input[i] == '\t')) // continue to find second operand
          continue;

        else if(comma && !sec_opr && input[i] != ' ' && input[i] != '\t') //  find second operand start
          sec_opr = TRUE;

        else if(sec_opr && (input[i] == ' ' || input[i] == '\t'))  //  find second operand end
          *oe = i - 1;
      }

      else if(*oe != NONE) {
        if(input[i] != ' ' && input[i] != '\t') {
          printf("label 존재 x, input[%d] = %c(%d) ", i, input[i], input[i]);
          printf("Assembly source file error at line %d\n", line);
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
    for(i = 0; i < INPUT_LEN; i++) {
      if(input[i] == '\0') {
        if(*ms != NONE && *me == NONE)  //  mnemonic만 있고 문자열이 끝날 경우
          *me = i - 1;
        else if(*os != NONE && *oe == NONE) //  operand까지 있고 문자열이 끝날 경우
          *oe = i - 1;
        break;
      } //  if-'\0' end

      if(*ls == NONE && input[i] != ' ' && input[i] != '\t')  //  find label start
        *ls = i;
      else if(*ls != NONE && *le == NONE) { //  finding label end
        if(input[i] == ' ' || input[i] == '\t') //  find label end
          *le = i - 1;
      }

      else if(*le != NONE && *ms == NONE) { //  finding mnemonic start
        if(input[i] != ' ' && input[i] != '\t') //  find mnemonic start
          *ms = i;
      }

      else if(*ms != NONE && *me == NONE) { //  finding mnemonic end
        if(input[i] == ' ' || input[i] == '\t') //  find mnemonic end
          *me = i - 1;
      } 

      else if(*me != NONE && *os == NONE) { //  finding operand start
        if(input[i] != ' ' && input[i] != '\t') //  find operand start
          *os = i;
      }

      else if(*os != NONE && *oe == NONE) { //  finding operand end
        if(input[i] == ' ' || input[i] == '\t') //  find operand end
          *oe = i;
      }

      else if(*oe != NONE) {
        if(input[i] != ' ' && input[i] != '\t') {
           printf("label 존재 o, input[%d] = %c(%d) ", i, input[i], input[i]);
          printf("Assembly source file error at line %d\n", line);
          return FALSE;
        }
      }
    } //  for-end

    for(i = *ls; i <= *le; i++)
      label[i - (*ls)] = input[i];
    input[i - (*ls)] = '\0';
  
    for(i = *ms; i <= *me; i++)
      mnemonic[i - (*ms)] = input[i];
    input[i - (*ms)] = '\0';
    
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

int pass_1(char* filename, char* lst_filename, FILE* fp_asm, FILE** fp_lst) {
  char input[INPUT_LEN];
  int ls = NONE, le = NONE; //  label start, end index
  int ms = NONE, me = NONE; //  opcode mnemonic start, end index
  int os = NONE, oe = NONE; //  operand start, end index 
  int i;

  change_extension(filename, lst_filename, ".lst"); 
  //
  printf("listing file name : %s\n", lst_filename);
  //

  *fp_lst = fopen(lst_filename, "w");
  if(!(*fp_lst)) {
    printf("listing file open error!\n");
    return FALSE;
  }

  /*
  fgets(input, INPUT_LEN, fp_asm);
  //
  printf("first line : %s", input);
  //
  */

  while(fgets(input, INPUT_LEN, fp_asm)) {
    input[strlen(input) - 1] = '\0';  //  '\n'키 제거
    line += 5;
  /*  
    if(input[0] == ' ' || input[0] == '\t')
      printf("%d : %s는 LABEL 없음\n", line, input);
    else
      printf("%d : %s", line, input);
  */
    if(!process_input_string(input, &ls, &le, &ms, &me, &os, &oe)) 
      return FALSE;
//    printf("%3d\t%s\t%s\t%s\n", line, label, mnemonic, operand);
    printf("%3d\t", line);
    if(input[0] == '.')
      printf("%s", input);

    if(ls != NONE && le != NONE) {
      for(i = ls; i <= le; i++)
        printf("%c", label[i - ls]);
      printf("\t");
    }
    else printf("\t");
    
    if(ms != NONE && me != NONE) {
      for(i = ms; i <= me; i++)
        printf("%c", mnemonic[i - ms]);
      printf("\t");
    }
    else printf("\t");

    if(os != NONE && oe != NONE) {
      for(i = os; i <= oe; i++)
        printf("%c", operand[i - os]);
      printf("\t");
    }
    else printf("\t");
    printf("\n");

    reset_indices(&ls, &le, &ms, &me, &os, &oe);
  }

  return TRUE;
}
