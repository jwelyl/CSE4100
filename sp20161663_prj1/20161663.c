#include "header.h"
#include "command.h"

int main(void) {
  char input[INPUT_LEN];
  char cmd[MAX_CMD];

  while(1) {
    printf("sicsim> ");
    
    fgets(input, INPUT_LEN, stdin);
    if(extract_command(input, cmd)) continue;  //  명령어가 유효하지 않을 경우

    if(!process_command(cmd, input)) break;
    else continue;
  }

  return 0;
}

