#include "link_loader.h"
#include "memory.h"
#include "estab.h"
#include "assemble.h" //  hex_to_dec 함수를 위한 include

int progaddr = 0;   //  program load address
int csaddr = 0;     //  control section address

//  0 ~ F까지에 대응하는 2의 보수
char two_compl[16] =
  {'F', 'E', 'D', 'C', 'B', 'A', '9', '8',
   '7', '6', '5', '4', '3', '2', '1', '0'};

void set_progaddr(int prog_addr) {
  progaddr = prog_addr;
}

int hex_ref_to_dec(char* hex_ref, int* ref_num) {
  if('0' <= hex_ref[1] && hex_ref[1] <= '9')
    *ref_num = hex_ref[1] - '0';
  else if('A' <= hex_ref[1] && hex_ref[1] <= 'F')
    *ref_num = hex_ref[1] - 'A' + 10;
  else if('a' <= hex_ref[1] && hex_ref[1] <= 'f')
    *ref_num = hex_ref[1] - 'a' + 10;
  else {
    printf("잘못된 reference number.\n");
    return FALSE;
  }
  if('0' <= hex_ref[0] && hex_ref[0] <= '9')
    *ref_num += ((hex_ref[0] - '0') * 16);
  else if('A' <= hex_ref[0] && hex_ref[0] <= 'F')
    *ref_num += ((hex_ref[0] - 'a' + 10) * 16);
  else if('a' <= hex_ref[0] && hex_ref[0] <= 'f')
    *ref_num += ((hex_ref[0] - 'a' + 10) * 16);
  else {
    printf("잘못된 reference number.\n");
    return FALSE;
  }
  return TRUE;
}

int neg_hex_to_dec(char* neg, int len, int* dec) {
  //  2's complement를 이용하여 음수 16진수 문자열을 10진수 음수로 변환
  //  neg가 음수이므로 가장 앞자리인 neg[0]는 '8' ~ 'F'임(8, 9, A, B, C, D, E, F)
  int i, idx, num = 16; 

  if('8' <= neg[0] && neg[0] <= '9') idx = neg[0] - '0'; 
  else if('A' <= neg[0] && neg[0] <= 'F') idx = neg[0] - 'A' + 10;
  else if('a' <= neg[0] && neg[0] <= 'f') idx = neg[0] - 'a' + 10;
  else {
    printf("유효하지 않은 16진수 음수\n");
    return FALSE;
  }
  neg[0] = two_compl[idx];

  for(i = 1; i < len; i++) {
    if('0' <= neg[i] && neg[i] <= '9') idx = neg[i] - '0';
    else if('A' <= neg[i] && neg[i] <= 'F') idx = neg[i] - 'A' + 10;
    else if('a' <= neg[i] && neg[i] <= 'f') idx = neg[i] - 'a' + 10;
    else {
      printf("유효하지 않은 16진수 음수\n");
      return FALSE;
    }

    neg[i] = two_compl[idx];
  }

  if('0' <= neg[len - 1] && neg[len - 1] <= '9')
    *dec = neg[len - 1] - '0';
  else if('A' <= neg[len - 1] && neg[len - 1] <= 'F')
    *dec = neg[len - 1] - 'A' + 10;
  else if('a' <= neg[len - 1] && neg[len - 1] <= 'f')
    *dec = neg[len - 1] - 'a' + 10;

  if(len >= 2) {
    for(i = len - 2; i >= 0; i--) {
      if('0' <= neg[i] && neg[i] <= '9')
        *dec += ((neg[i] - '0') * num);
      else if('A' <= neg[i] && neg[i] <= 'F')
        *dec += ((neg[i] - 'A' + 10) * num);
      else if('a' <= neg[i] && neg[i] <= 'f')
        *dec += ((neg[i] - 'a' + 10) * num);
      num *= 16;
    }
  }
  *dec += 1;  //  음수 16진수 문자열을 양수 정수로 변환함
  *dec *= -1; //  -1을 곱해서 다시 음수 정수로 변환함

  return TRUE;
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
  char hex[3];  //  2 digit 16진수 reference number, length, 등등
  int add, len, start, mod, cur;
  char op;

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
      } //  H record end

      else if(input[0] == 'R') {  //  reference record일 경우
        int ref_num;

        for(j = 0; j < 11; j++) {
          // 10진수라 가정
          if(8 * j + 1 > strlen(input) - 1)  //  더 이상 reference가 없을 경우
            break;
          
          hex[0] = input[8 * j + 1];
          hex[1] = input[8 * j + 2];
          hex[2] = '\0';

          if(!hex_ref_to_dec(hex, &ref_num) || ref_num >= 14) {
            printf("Reference 번호 오류\n");
            return FALSE;
          } 

          for(k = 8 * j + 3; k <= 8 * j + 8; k++) { //  reference를 해당 번호에 복사
            if(input[k] == ' ' || input[k] == '\0') break;
            temp[k - 8 * j - 3] = input[k];
          }
          temp[k - 8 * j - 3] = '\0';
          strcpy(reference[ref_num], temp);
        } //  for-j end
      } //  R record end

      else if(input[0] == 'T') {  //  text record일 경우
        //  text record 시작 주소 결정
        for(j = 1; j <= 6; j++)
          temp[j - 1] = input[j];
        temp[j - 1] = '\0';
        hex_to_dec(temp, &add);
        
        //  text record 길이 결정
        hex[0] = input[7];
        hex[1] = input[8];
        hex[2] = '\0';
        hex_ref_to_dec(hex, &len);

        start = csaddr + add;
        cur = start;

        if(len > 30) {
          printf("Text record length error!\n");
          return FALSE;
        }

        for(j = 9; j <= 67; j += 2) {
          int dec;  //  16진수 문자열(1bit)을 정수로 변환

          if(j >= 9 + len * 2) break;
          hex[0] = input[j];
          hex[1] = input[j + 1];
          hex[2] = '\0';

          if(('8' <= hex[0] && hex[0] <= '9') || ('A' <= hex[0] && hex[0] <= 'F') ||
            ('a' <= hex[0] && hex[0] <= 'f')) { //  해당 값이 음수일 경우
            
            if(!neg_hex_to_dec(hex, 2, &dec)) {
              printf("pass2 과정에서 음수 변환 실패\n");
              return FALSE;
            }
          }
          else {  //  양수일 경우
            if('0' <= hex[1] && hex[1] <= '9')
              dec = hex[1] - '0';
            else if('A' <= hex[1] && hex[1] <= 'F')
              dec = hex[1] - 'A' + 10;
            else if('a' <= hex[1] && hex[1] <= 'f')
              dec = hex[1] - 'a' + 10;

            dec += (hex[0] - '0') * 16; 
          }
          
          memory[cur++] = dec;  //  가상 메모리에 저장
        } //  text record for end
      } //  Text record end

      else if(input[0] == 'M') {  //  modification record일 경우
        //  modification 위치 결정
        int ref_num, addr;
        char c[3];
        char temp5[6], temp6[7];   //  modification 길이가 각각 5, 6일때 사용
        char temp8[9];            //  sprintf 위한 배열

        for(j = 1; j <= 6; j++) 
          temp[j - 1] = input[j];
        temp[j - 1] = '\0';
        hex_to_dec(temp, &mod);
        
        //  수정할 길이 결정
        hex[0] = input[7];
        hex[1] = input[8];
        hex[2] = '\0';
        hex_ref_to_dec(hex, &len);

        op = input[9];  //  operator

        for(j = 10; j <= 11; j++)
          hex[j - 10] = input[j];
        hex[j - 10] = '\0';
        hex_ref_to_dec(hex, &ref_num);
        mod += csaddr;  //  실제로 수정할 위치

        for(k = 0; k < 3; k++) {
          c[k] = memory[mod + k];
          sprintf(temp8, "%8X", c[k]);
          temp[2 * k] = temp8[6];
          temp[2 * k + 1] = temp8[7];
        }
        temp[6] = '\0';
        
        for(k = 0; k < 6; k++) {  //  빈 자리 0으로 채우기
          if(temp[k] == ' ')
            temp[k] = '0';
        }

        if(len == 6) {
          for(k = 0; k < 6; k++)
            temp6[k] = temp[k];
          temp6[k] = '\0';

          if(('8' <= temp6[0] && temp6[0] <= '9') || ('A' <= temp6[0] && temp6[0] <= 'F') ||
             ('a' <= temp6[0] && temp6[0] <= 'f'))  //  수정할 부분이 음수일 경우
            neg_hex_to_dec(temp6, 6, &addr);
          else
            hex_to_dec(temp6, &addr); 
        }
        else if(len == 5) {
          for(k = 0; k < 5; k++)
            temp5[k] = temp[k + 1];
          temp5[k] = '\0';
          
          if(('8' <= temp5[0] && temp5[0] <= '9') || ('A' <= temp5[0] && temp5[0] <= 'F') ||
             ('a' <= temp5[0] && temp5[0] <= 'f'))  //  수정할 부분이 음수일 경우
            neg_hex_to_dec(temp5, 6, &addr);
          else 
            hex_to_dec(temp5, &addr); 
        }

        find_sym_addr(reference[ref_num], &add);
        if(op == '+') addr += add;
        else if(op == '-') addr -= add;

        sprintf(temp8, "%8X", addr);
        for(k = 0; k < 8; k++) {  //  빈 공간 채우기
          if(temp8[k] == ' ')
            temp8[k] = '0';
        }
        if(len == 5) {
          for(k = 1; k < 6; k++)
            temp[k] = temp8[k + 2];
        }
        else if(len == 6) {
          for(k = 0; k < 6; k++)
            temp[k] = temp8[k + 2];
        } 

        for(k = 0; k < 3; k++) {
          int renew;  //  갱신된 메모리 값
          hex[0] = temp[2 * k];
          hex[1] = temp[2 * k + 1];
          hex[2] = '\0';

          if(('8' <= hex[0] && hex[0] <= '9') || ('A' <= hex[0] && hex[0] <= 'F') ||
             ('a' <= hex[0] && hex[0] <= 'f')) {  //  메모리 값이 음수일 경우
            neg_hex_to_dec(hex, 2, &renew); 
          }
          else
            hex_to_dec(hex, &renew);
          memory[mod + k] = renew;  //  메모리에 값 갱신
        }
      } //  Modification record end
    } //  while end
  } //  for three file end
  
  for(i = 0; i < REFERENCE_N; i++) {
    free(reference[i]);
    reference[i] = NULL;
  }
  print_loadmap();

  return TRUE;
}
