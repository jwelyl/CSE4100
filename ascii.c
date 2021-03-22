#include <stdio.h>

int main(void) {
  
  FILE* fp = fopen("ascii.txt", "w");      
  int i;

  for(i = 32 ; i <= 126; i++) {
    fprintf(fp, "char : %c | dec : %d | hex : %X\n", i, i, i);
  }
  fclose(fp);

  return 0;
}
