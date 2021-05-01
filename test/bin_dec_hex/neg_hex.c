#include <stdio.h>

int main(void) {
  char temp[9];
  char arr[3];
  int neg = -57;
  printf("%d(%X)\n", neg, neg);
  
  sprintf(temp, "%X", neg);
  arr[0] = temp[6];
  arr[1] = temp[7];
  arr[2] = '\0';
  printf("%s\n", arr);  
  return 0;
}
