#include <stdio.h>

int main(void) {
  int i;

  char arr[10] = {0, };

  for(i = 0; i < 10; i++) {
    if(arr[i] == 0) printf("null ");
    else printf("%d ", arr[i]);
  }
  printf("\n");
  return 0;

}
