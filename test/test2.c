#include <stdio.h>
#include <string.h>

int main(void) {
  char arr[100];

  fgets(arr, 100, stdin);

  arr[strlen(arr) - 1] = 0;
  printf("%s(%zu)\n", arr, strlen(arr));
  return 0;
}
