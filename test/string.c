#include <stdio.h>
#include <string.h>

#define MAX_INPUT   100
#define MAX_CMD      11
#define MAX_OPR      11
#define MNEMONIC      7

void clearReadBuffer() {
  while(getchar() != '\n');
}

int main(void) {
  char input[MAX_INPUT];
  char cmd[MAX_CMD];
  char opr1[MAX_OPR], opr2[MAX_OPR], opr3[MAX_OPR];
  char mnemonic[MNEMONIC];

  int i;
  while(1) {
    printf("sicsim> ");

    fgets(input, MAX_INPUT, stdin);
    int inputOver = 1;
    int cmdLenOver = 0;
    for(i = 0; i < MAX_INPUT; i++) {
      if(input[i] == '\n') {
        input[i] = '\0';
        inputOver = 0;
        break;
      }
    }

    if(inputOver) clearReadBuffer();
    printf("입력한 문장 : %s\n", input);
   
    i = 0; 
    while(1) {
      if(input[i] == ' ' || input[i] == '\0') {
        cmd[i] = '\0';
        break;
      }
      else if(i == MAX_CMD - 1) {
        cmdLenOver = 1;
        cmd[i] = '\0';
        break;
      }
      cmd[i] = input[i];
      i++;
    }
    if(!cmdLenOver) { //  유효한 명령어일 경우 
      printf("입력한 명령어 : %s\n", cmd); 
      
    }
  }
}
