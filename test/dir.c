#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

int main(void) {
  DIR* dp = NULL;
  struct dirent* dir_entry;
  struct stat buf;

  if((dp = opendir(".")) == NULL) {
    printf("현재 디렉토리를 열 수가 없습니다...\n");
    return -1;
  }
  
  while(dir_entry = readdir(dp)) {
    lstat(dir_entry->d_name, &buf);
    
    if(S_ISDIR(buf.st_mode))
      printf("%s/  ", dir_entry->d_name);
    else {
      if((buf.st_mode) & S_IXUSR)
        printf("%s*  ", dir_entry->d_name);
      else
        printf("%s  ", dir_entry->d_name);
    }
  }
  printf("\n");
  
  closedir(dp);
  return 0;
}
