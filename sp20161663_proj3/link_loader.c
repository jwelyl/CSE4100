#include "link_loader.h"
#include "memory.h"
#include "estab.h"

int progaddr = 0;   //  program load address
int csaddr = 0;     //  control section address

void set_progaddr(int prog_addr) {
  progaddr = prog_addr;
}

int loader_pass1(FILE** fp_obj1, FILE** fp_obj2, FILE** fp_obj3) {
  char input[INPUT_LEN];
  int i;
  FILE* fp;

  for(i = 0; i < 3; i++) {  //  최대 3개의 object file
    fp = i % 3 == 0 ? *fp_obj1 : (i % 3 == 1 ? *fp_obj2 : *fp_obj3);
    
    while(fgets(input, INPUT_LEN, fp)) {
      input[strlen(input) - 1] = '\0';  //  '\n' 키 제거

      if(input[0] == 'H') { //  header record일 경우
      
      }
      else if(input[0] == 'D') {  //  Define record일 경우
      
      }
      else continue;
    }
  }

  return TRUE; 
}

int loader_pass2(FILE** fp_obj1, FILE** fp_obj2, FILE** fp_obj3) {
  
  return TRUE;
}
