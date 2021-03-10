#include "header.h"
#include "command.h"

int main(void) {
  char cmd[CMD_LEN];

  while(1) {
    printf("sicsim> ");
    scanf("%s", cmd);
    
    if(!process_command(cmd)) break;
    else continue;
  }

  return 0;
}

