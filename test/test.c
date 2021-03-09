#include <stdio.h>

int main(void) {
  FILE* fp = NULL;
  char buffer[10];

  if((fp = fopen("test.txt", "r")) == NULL) {
    printf("파일을 열 수 없습니다...\n");
    return -1;
  }

  while(fgets(buffer, 10, fp) != NULL)
    printf("%s", buffer);

  fclose(fp);
  return 0;
}
