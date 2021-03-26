#include "20161663.h"
#include "optable.h"
#include "command.h"

int main(void) {
  char input[INPUT_LEN];
  char cmd[MAX_CMD];
  int opt_start;  //  명령어(cmd)가 끝나고 option 찾기 시작하는 인덱스
  make_optable();
  
  while(1) {
    printf("sicsim> ");
    
    fgets(input, INPUT_LEN, stdin);
    if(invalid_command(input, cmd, &opt_start)) continue;  //  명령어가 유효하지 않을 경우

    if(!process_command(cmd, input, opt_start)) break;
    else continue;
  }

  return 0;
}

