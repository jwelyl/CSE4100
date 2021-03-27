#include "assemble.h"

int line = 0;
int LOCCTR = 0;
char label[STRING_SIZE];
char opcode[STRING_SIZE];
char operand[STRING_SIZE];

int pass_1(char* filename, FILE* fp_asm) {
  int i, error_flag = FALSE;
  char lst_filename[FILENAME] = {0, };
  char input[INPUT_LEN];
  FILE* fp_lst = NULL;

  for(i = 0; i < strlen(filename) - 4; i++)
    lst_filename[i] = filename[i];
  strcat(lst_filename, ".lst");

  //
  printf("listing file name : %s\n", lst_filename);
  //

  fp_lst = fopen(lst_filename, "w");
  if(!fp_lst) {
    printf("listing file open error!\n");
    return FALSE;
  }



  if(error_flag) {
    remove(lst_filename);
    return FALSE;
  }

  return TRUE;
}
