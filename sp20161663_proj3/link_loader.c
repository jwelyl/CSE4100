#include "link_loader.h"
#include "memory.h"
#include "estab.h"
#include "assemble.h" //  hex_to_dec 함수를 위한 include

int progaddr = 0;   //  program load address
int csaddr = 0;     //  control section address

void set_progaddr(int prog_addr) {
  progaddr = prog_addr;
}

int loader_pass1(FILE* fp_obj1, FILE* fp_obj2, FILE* fp_obj3) {
  char input[INPUT_LEN];
  int i, j, k;
  FILE* fp;
  char symbol[SYMBOL_SIZE] = {0, };  //  csect name 또는 symbol name 저장할 배열
  char temp[SYMBOL_SIZE] = {0, };   //  16진수 address, 길이를 10진수로 변환하기 위한 배열
  int address = 0;
  int length;

  csaddr = progaddr;  //  첫 번째 control section address = program address

  for(i = 0; i < 3; i++) {  //  최대 3개의 object file
    fp = (i % 3 == 0) ? fp_obj1 : ((i % 3 == 1 ? fp_obj2 : fp_obj3));
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
        
        if(find_symbol(symbol)) {
          printf("이미 존재하는 symbol!\n");
          return FALSE;          
        }
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
    } //  while end
  } //  for 3 file end

  //
  print_loadmap();
  //
  return TRUE; 
}

int loader_pass2(FILE* fp_obj1, FILE* fp_obj2, FILE* fp_obj3) {
  char input[INPUT_LEN];
  int i, j, k;
  FILE* fp;
  char symbol[SYMBOL_SIZE] = {0, };  //  csect name 또는 external symbol name 저장할 배열
  char* reference[REFERENCE_N];      //  reference를 번호에 맞게 저장할 배열
                                     //  index : reference 번호(ex. reference[2] : 02 reference)
  char temp[SYMBOL_SIZE];

  for(i = 0; i < REFERENCE_N; i++)
    reference[i] = (char*)malloc(sizeof(char) * SYMBOL_SIZE);
  
  for(i = 0; i < 3; i++) {  //  최대 3개의 object file
    fp = (i % 3 == 0) ? fp_obj1 : ((i % 3 == 1 ? fp_obj2 : fp_obj3));
    if(!fp) break;  //  첫 번째 파일부터 순서대로 확인, fp가 NULL일 경우 file이 없으므로 break
 
    while(fgets(input, INPUT_LEN, fp)) {
      input[strlen(input) - 1] = '\0';  //  '\n' 키 제거

      if(input[0] == 'H') { //  header record일 경우
                            //  csect 주소를 얻어오기 위해 검사
        for(j = 1; j <= 6; j++) {   //  csect name 저장 
          if(input[j] == ' ') break;
          symbol[j - 1] = input[j];
        }
        symbol[j - 1] = '\0';
      
        if(!find_sym_addr(symbol, &csaddr)) {
          printf("존재하지 않는 symbol\n");
          return FALSE;
        }
        strcpy(reference[1], symbol);
        //
        printf("%d CSECT : %s\n  ADDRESS : %X\n", i + 1, symbol, csaddr);
        //
      } //  H record end

      else if(input[0] == 'R') {  //  reference record일 경우
        int ref_num;

        for(j = 0; j < 11; j++) {
          // 10진수라 가정
          if(8 * j + 1 > strlen(input) - 1)  //  더 이상 reference가 없을 경우
            break;
                
          ref_num = input[8 * j + 2] - '0';
          ref_num += ((input[8 * j + 1] - '0') * 10);

          for(k = 8 * j + 3; k <= 8 * j + 8; k++) { //  reference를 해당 번호에 복사
            if(input[k] == ' ' || input[k] == '\0') break;
            temp[k - 8 * j - 3] = input[k];
          }
          temp[k - 8 * j - 3] = '\0';
          strcpy(reference[ref_num], temp);
        } //  for-j end
      } //  R record end
    } //  while end

    int l;
    for(l = 0; l < REFERENCE_N; l++) {
      printf("%d : %s\n", l, reference[l]);
    }
  } //  for three file end
  
  for(i = 0; i < REFERENCE_N; i++) {
    free(reference[i]);
    reference[i] = NULL;
  }

  return TRUE;
}

 /*
          // 16진수라 가정
          if('0' <= input[8 * j + 2] && input[8 * j + 2] <= '9')
            ref_num = input[8 * j + 2] - '0';
          else if('A' <= input[8 * j + 2] && input[8 * j + 2] <= 'F')
            ref_num = input[8 * j + 2] - 'A' + 10;
          else if('a' <= input[8 * j + 2] && input[8 * j + 2] <= 'f')
            ref_num = input[8 * j + 2] - 'a' + 10;
          else {
            printf("잘못된 reference number.\n");
            return FALSE;
          }
          if('0' <= input[8 * j + 1] && input[8 * j + 1] <= '9')
            ref_num += ((input[8 * j + 1] - '0') * 16);
          else if('A' <= input[8 * j + 1] && input[8 * j + 1] <= 'F')
            ref_num += ((input[8 * j + 1] - 'a' + 10) * 16);
          else if('a' <= input[8 * j + 1] && input[8 * j + 1] <= 'f')
            ref_num += ((input[8 * j + 1] - 'a' + 10) * 16);
          else {
            printf("잘못된 reference number.\n");
            return FALSE;
          }
        */

