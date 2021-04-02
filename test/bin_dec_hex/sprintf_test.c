#include <stdio.h>
#include <string.h>

int main(void) {
  char arr[10];
  int num = 39;

  for(int i = 0; i < 10; i++) arr[i] = '0';
  arr[9] = '\0';

  sprintf(arr, "%7X", num);
  printf("%s\n", arr);
  for(int i = 0; i < 10; i++)
    printf("%c %d\n", arr[i], arr[i]);

  return 0;
}
