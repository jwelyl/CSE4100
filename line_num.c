#include <stdio.h>
#include <string.h>

int main(void) {
  char filename[30] = {0, };
  char lst_filename[30] = {0, };
  char extension[5] = {0, };
  char input[100];
  int i;
  int line = 0;
  FILE* fp_asm = NULL;
  FILE* fp_lst = NULL;

  printf("input file name (asm) : ");
  scanf("%s", filename);
  
  for(i = strlen(filename) - 4; i < strlen(filename); i++)
    extension[i - strlen(filename) + 4] = filename[i];

  if(strcmp(extension, ".asm")) {
    printf("잘못된 확장자\n");
    return -1;
  }

  fp_asm = fopen(filename, "r");
  for(i = 0; i < strlen(filename) - 4; i++)
    lst_filename[i] = filename[i];
  strcat(lst_filename, ".lst");

  fp_lst = fopen(lst_filename, "w");
  
  while(fgets(input, 100, fp_asm)) {
    input[strlen(input) - 1] = '\0';
    line += 5;

    fprintf(fp_lst, "%3d\t%s\n", line, input);
  }
  
  fclose(fp_asm);
  fclose(fp_lst);

  return 0;
}
