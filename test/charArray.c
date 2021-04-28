#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) {
  char* ptr[3];
  char str1[15] = "I am a boy.";
  char str2[20] = "Sex is the best!";
  char str3[30] = "I like it.";
  int i;

  for(i = 0; i < 3; i++)
    ptr[i] = (char*)malloc(sizeof(char) * 100);

  strcpy(ptr[0], str1);
  strcpy(ptr[1], str2);
  strcpy(ptr[2], str3);

  for(i = 0; i < 3; i++)
    printf("%s\n", ptr[i]);

  return 0;
}
