#include "20161663.h"
#include "optable.h"
#include "command.h"

char memory[MAX_MEM_SIZE];

int main(void) {
  char input[INPUT_LEN];
  char cmd[MAX_CMD];

  make_optable();
  
  while(1) {
    printf("sicsim> ");
    
    fgets(input, INPUT_LEN, stdin);
    if(invalid_command(input, cmd)) continue;  //  명령어가 유효하지 않을 경우

    if(!process_command(cmd, input, memory)) break;
    else continue;
  }

  return 0;
}

