#include "link_loader.h"
#include "memory.h"
#include "estab.h"
#include "assemble.h" //  hex_to_dec 함수를 위한 include

int progaddr = 0;   //  program load address
int csaddr = 0;     //  control section address

void set_progaddr(int prog_addr) {
  progaddr = prog_addr;
}

int loader_pass1(FILE** fp_obj1, FILE** fp_obj2, FILE** fp_obj3) {
  char input[INPUT_LEN];
  int i, j, k;
  FILE* fp;
  char symbol[SYMBOL_SIZE] = {0, };  //  csect name 또는 symbol name 저장할 배열
  char temp[SYMBOL_SIZE] = {0, };   //  16진수 address, 길이를 10진수로 변환하기 위한 배열
  int address = 0;
  int length;

  csaddr = progaddr;  //  첫 번째 control section address = program address

  for(i = 0; i < 3; i++) {  //  최대 3개의 object file
    fp = (i % 3 == 0) ? *fp_obj1 : ((i % 3 == 1 ? *fp_obj2 : *fp_obj3));
    if(!fp) break;  //  첫 번째 파일부터 순서대로 확인, fp가 NULL일 경우 file이 없으므로 break
    
    while(fgets(input, INPUT_LEN, fp)) {
      input[strlen(input) - 1] = '\0';  //  '\n' 키 제거

      if(input[0] == 'H') { //  header record일 경우
        for(j = 1; j <= 6; j++) {   //  csect name 저장 
          if(input[j] == ' ') break;
          symbol[j - 1] = input[j];
        }
        symbol[j - 1] = '\0';
        
        if(i == 0) {  //  첫 번째 파일일 경우
          for(j = 7; j <= 12; j++)  //  csect address 저장
            temp[j - 7] = input[j];
          temp[j - 7] = '\0';

          hex_to_dec(temp, &address); //  address에 temp를 10진수 정수로 변환하여 저장
          csaddr += address;  //  csaddr 갱신
        }
        else  //  두 번째 이후 파일일 경우
          csaddr += length; //  이전 control section의 길이를 더함
       
        for(j = 13; j <= 18; j++)  //  길이 저장
          temp[j - 13] = input[j];
        temp[j - 13] = '\0';
        hex_to_dec(temp, &length);  //  length에 temp를 10진수 정수로 변환하여 저장
        push_est_node(symbol, csaddr, length);
      }
      else if(input[0] == 'D') {  //  Define record일 경우
        for(j = 0; j < 6; j++) {
          if(input[12 * j + 1] == '\0')  //  더 이상 symbol이 없을 경우
            break;
          
          for(k = 12 * j + 1; k <= 12 * j + 6; k++) { //  symbol name 복사
            if(input[k] == ' ') break;
            symbol[k - 12 * j - 1] = input[k]; 
          }
          symbol[k - 12 * j - 1] = '\0';
        
          for(k = 12 * j + 7; k <= 12 * j + 12; k++)  //  address 복사
            temp[k - 12 * j - 7] = input[k];
          temp[k - 12 * j - 7] = '\0';
        
          hex_to_dec(temp, &address); //  symbol의 상대적인 주소 구하기
          address += csaddr;  //  symbol의 실제 주소 구하기
          push_est_node(symbol, address, NONE);  //  control sect 아닌 symbol의 경우 길이 -1
        } 
      }
      else continue;
    }
  }

  //
  print_loadmap();
  //
  return TRUE; 
}

int loader_pass2(FILE** fp_obj1, FILE** fp_obj2, FILE** fp_obj3) {
  
  return TRUE;
}
