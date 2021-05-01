#include <stdio.h>

int main(void) {
  int i;

  for(i = -128; i <= 127; i++)
    printf("%c(%d)[%#X] [%#x]\n", i, i, i, i);

  int neg = -3989481;
  printf("%d[%#X] [%#x]\n", neg, neg, neg);

  return 0;
}
