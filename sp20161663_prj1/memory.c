#include "memory.h"

void reset_to_zero(char* hex, int len) {  //  16진수 0으로 초기화
  int i;
  
  for(i = 0; i < len - 1; i++)
    hex[i] = '0';
  hex[i] = '\0';
}

void dec_to_hex(int dec, char* hex, int len) {
  int i, r;

  reset_to_zero(hex, len);
  i = len - 2;

  while(TRUE) {
    r = dec % 16;
    
    if(0 <= r && r <= 9)
      hex[i] = '0' + r;
    else if(10 <= r && r <= 15)
      hex[i] = 'A' + r - 10;

    dec /= 16;
    if(dec == 0) break;
    i--;
  }
}

void reset_memory() {
  int i = 0;
  for(i = 0; i < MAX_MEM_SIZE; i++)
    memory[i] = 0;
}

void dump_memory(int start, int end) {
  int i, j;
  int start_line = start / 16;
  int end_line = end / 16;

  int addr;     //  각 라인의 시작 주소(0, 16, 32, ...)
  int addr_end; //  각 라인의 끝 주소(15, 31, 47, ...)

  char addr_col[ADDRESS_COL];
  char hex_col[HEX_COL];

  for(i = start_line; i <= end_line; i++) {
    addr = i * 16;
    addr_end = addr + 15;
  
    dec_to_hex(addr, addr_col, ADDRESS_COL);
    printf("%s ", addr_col);

    if(i == start_line && i == end_line) {  //  한 줄 출력
      for(j = addr; j < start; j++)
        printf("   ");
  
      for(j = start; j <= end; j++) {
        dec_to_hex(memory[j], hex_col, HEX_COL);
        printf("%s ", hex_col);
      }

      for(j = end + 1; j <= addr_end; j++) 
        printf("   ");
      printf("; ");

      for(j = addr; j < start; j++)
        printf(".");
  
      for(j = start; j <= end; j++) {
        if(32 <= memory[j] && memory[j] <= 126) {
          printf("%c", memory[j]);
        }
        else printf(".");
      }

      for(j = end + 1; j <= addr_end; j++) 
        printf(".");
      printf("\n");
      break;
    }

    else if(i == start_line) {  //  시작줄일 경우
       for(j = addr; j < start; j++)
        printf("   ");
  
      for(j = start; j <= addr_end; j++) {
        dec_to_hex(memory[j], hex_col, HEX_COL);
        printf("%s ", hex_col);
      }
      printf("; ");

      for(j = addr; j < start; j++)
        printf(".");
  
      for(j = start; j <= addr_end; j++) {
        if(32 <= memory[j] && memory[j] <= 126) {
          printf("%c", memory[j]);
        }
        else printf(".");
      }
      printf("\n");
    }

    else if(i == end_line) {  //  마지막줄일 경우
      for(j = addr; j <= end; j++) {
        dec_to_hex(memory[j], hex_col, HEX_COL);
        printf("%s ", hex_col);
      }
      for(j = end + 1; j <= addr_end; j++) 
        printf("   ");
      printf("; ");
  
      for(j = addr; j <= end; j++) {
        if(32 <= memory[j] && memory[j] <= 126) {
          printf("%c", memory[j]);
        }
        else printf(".");
      }
       for(j = end; j < addr_end; j++)
        printf(".");

      printf("\n");
    }

    else {  //  중간 줄일 경우
      for(j = addr; j <= addr_end; j++) {
        dec_to_hex(memory[j], hex_col, HEX_COL);
        printf("%s ", hex_col);
      }
      printf("; ");
  
      for(j = addr; j <= addr_end; j++) {
        if(32 <= memory[j] && memory[j] <= 126) {
          printf("%c", memory[j]);
        }
        else printf(".");
      }
      printf("\n");
    }
  } //  for - end
}

void edit_value(int addr, int val) {
  memory[addr] = val;
}

void fill_value(int start, int end, int val) {
  int i;

  for(i = start; i <= end; i++)
    memory[i] = val;
}
