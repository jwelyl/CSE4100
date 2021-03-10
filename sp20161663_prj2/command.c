#include "command.h"
#include "queue.h"

int process_command(char* cmd) {
  DIR* dp = NULL;
  struct dirent* dir_entry;
  struct stat dir_stat;
 
  if(!strcmp(cmd, "quit") || !strcmp(cmd, "q")) { 
    deleteQueue();
    return 0;
  }
 
  if(!strcmp(cmd, "help") || !strcmp(cmd, "h")) {
    enqueue(cmd);

    printf("\nh[elp]");
    printf("\nd[ir]");
    printf("\nq[uit]");
    printf("\nhi[story]");
    printf("\ndu[mp] [start, end]");
    printf("\ne[dit] address, value");
    printf("\nf[ill] start, end, value");
    printf("\nreset");
    printf("\nopcode mnemonic");
    printf("\nopcodelist\n\n");
  }
  else if(!strcmp(cmd, "dir") || !strcmp(cmd, "d")) {
    enqueue(cmd);

    if((dp = opendir(".")) == NULL) {
      printf("현재 디렉토리를 열 수가 없습니다.\n");
      exit(-1);
    }
    
//    int cnt = 0;

    while((dir_entry = readdir(dp))) {
//      cnt++;
      lstat(dir_entry->d_name, &dir_stat);

//      if(cnt % 4 == 1) printf("          ");

      if(S_ISDIR(dir_stat.st_mode))
        printf("%s/  ", dir_entry->d_name);
      else {
        if((dir_stat.st_mode) & S_IXUSR)
          printf("%s*  ", dir_entry->d_name);
        else
          printf("%s  ", dir_entry->d_name);
      }
//      if(cnt % 4 == 0) printf("\n");
    }
//    if(cnt % 4 != 0) 
    printf("\n");
  }
  else if(!strcmp(cmd, "history") || !strcmp(cmd, "hi")) {
    enqueue(cmd);
    printQueue();
  }
  else if(!strcmp(cmd, "dump") || !strcmp(cmd, "du")) {
    enqueue(cmd);
    printf("%s는 구현 예정\n", cmd);
  }
  else if(!strcmp(cmd, "edit") || !strcmp(cmd, "e")) {
    enqueue(cmd);
    printf("%s는 구현 예정\n", cmd);
  }
  else if(!strcmp(cmd, "fill") || !strcmp(cmd, "f")) {
    enqueue(cmd);
    printf("%s는 구현 예정\n", cmd);
  }
  else if(!strcmp(cmd, "reset")) {
    enqueue(cmd);
    printf("%s는 구현 예정\n", cmd);
  }
  else if(!strcmp(cmd, "opcode")) {
    enqueue(cmd);
    printf("%s는 구현 예정\n", cmd);
  }
  else if(!strcmp(cmd, "opcodelist")) {
     enqueue(cmd);
     printf("%s는 구현 예정\n", cmd);
  }

  return 1;
}

