#include <stdio.h>
#include <string.h>

int main(void) {
  int dec = 317;
  char arr[30];

  sprintf(arr, "%c %X %f %e", 'a', 10, 3.2f, 1.123456e-21f);
  printf("%s\n", arr);
  
  sprintf(arr, "%X", dec);
  printf("%s\n", arr);

  dec *= -1;
  sprintf(arr, "%X", dec);
  printf("%s\n", arr);

  for(int i = 0; i < 30; i++) {
    printf("arr[%d] = %c\n", i, arr[i]);
  }

  memset(arr, 0, sizeof(arr));
  dec = -2342143;
  sprintf(arr, "%X", dec);
  printf("%s\n", arr);

  for(int i = 0; i < 30; i++) {
    printf("arr[%d] = %c\n", i, arr[i]);
  }


  return 0;
}
