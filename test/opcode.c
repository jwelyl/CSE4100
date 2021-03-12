#include <stdio.h>
#include <string.h>
#define OPCODE_INPUT  30
#define TRUE  1
#define FALSE 0
#define OPCODE  3
#define MNEMONIC  7
#define OPTABLE_SIZE  20

int mnemonic_sum(char* mnemonic) {
  int i = strlen(mnemonic);
  int num = 1;
  int ret = 0;

  for(; i >= 0; i--) {
    ret += ((mnemonic[i] - 'A') * num);
    num *= 26;
  }
   
  return ret;
}

int hash_function(char* mnemonic) {
  int len = strlen(mnemonic);
  int sum = mnemonic_sum(mnemonic);

  FILE* fp = fopen("num_of_opcodes.txt", "a");

  fprintf(fp, "%d\n", len);
  if(len == 1) return 1;
  else if(len == 6) return 11;
  else if(len == 3) {
    if(2041 <= sum && sum <= 58617) return 0;
    else if(93821 <= sum && sum <= 128739) return 1;
    else if(161239 <= sum && sum <= 166049) return 2;
    else if(195299 <= sum && sum <= 195429) return 3;
    else if(195583 <= sum && sum <= 195793) return 4;
    else if(195897 <= sum && sum <= 224653) return 5;
    else if(307203 <= sum && sum <= 328731) return 6;
    else if(329147 <= sum && sum <= 329277) return 7;
    else if(329355 <= sum && sum <= 329615) return 8;
    else if(329641 <= sum && sum <= 329849) return 9;
    else if(330551 <= sum && sum <= 339885) return 10;
  }
  else if(len == 4) {
    if(54821 <= sum && sum <= 55133) return 11;
    else if(1168453 <= sum && sum <= 1526109) return 12;
    else if(4442633 <= sum && sum <= 5842733) return 13;
    else if(5843045 <= sum && sum <= 8098441) return 14;
    else if(8560981 <= sum && sum <= 8577829) return 15;
    else if(8578141 <= sum && sum <= 8839077) return 16;
  }
  else if(len == 2 || len == 5) {
    if(9841 <= sum && sum <= 12857) return 17;
    else if(14885 <= sum && sum <= 30381533) return 18;
    else if(30381845 <= sum && sum <= 64680109) return 19;
  }

}

int main(void) {
  FILE* fp = fopen("opcode.txt", "r");
  FILE* fp1 = fopen("opcode_1.txt", "w");
  FILE* fp2 = fopen("opcode_2.txt", "w");
  FILE* fp3 = fopen("opcode_3.txt", "w");
  FILE* fp4 = fopen("opcode_4.txt", "w");
  FILE* fp5 = fopen("opcode_5.txt", "w");
  FILE* fp6 = fopen("opcode_6.txt", "w");

  char input[OPCODE_INPUT];
  char opcode[OPCODE];
  char mnemonic[MNEMONIC];
  int i, mstart;
  int len;
  int optable[OPTABLE_SIZE] = {0, };
  int mnem_sum;

  opcode[OPCODE - 1] = '\0';
  mnemonic[MNEMONIC - 1] = '\0';

  while(TRUE) {
    if(!fgets(input, OPCODE_INPUT, fp)) break;

//    printf("%s", input);

    for(i = 0; i < 2; i++)
      opcode[i] = input[i];
    for(i = 2; ; i++) {
      if(input[i] != ' ' && input[i] != '\t') {
        mstart = i;
        break;
      }
    }

    for(i = mstart; ; i++) {
      if(input[i] == ' ' || input[i] == '\t') {
        mnemonic[i - mstart] = '\0';
        break;
      }
      mnemonic[i - mstart] = input[i];
    }

    mnem_sum = mnemonic_sum(mnemonic);
    int len = strlen(mnemonic);

    if(len == 1) fprintf(fp1, "opcode : %s, mnemonic : %s(%d), sum : %d\n", opcode, mnemonic, len, mnem_sum);
    else if(len == 2) fprintf(fp2, "opcode : %s, mnemonic : %s(%d), sum : %d\n", opcode, mnemonic, len, mnem_sum);
    else if(len == 3) fprintf(fp3, "opcode : %s, mnemonic : %s(%d), sum : %d\n", opcode, mnemonic, len, mnem_sum);
    else if(len == 4) fprintf(fp4, "opcode : %s, mnemonic : %s(%d), sum : %d\n", opcode, mnemonic, len, mnem_sum);
    else if(len == 5) fprintf(fp5, "opcode : %s, mnemonic : %s(%d), sum : %d\n", opcode, mnemonic, len, mnem_sum);
    else if(len == 6) fprintf(fp6, "opcode : %s, mnemonic : %s(%d), sum : %d\n", opcode, mnemonic, len, mnem_sum);

    optable[hash_function(mnemonic)] += 1;
  }

  for(i = 0; i < OPTABLE_SIZE; i++)
    printf("%d ", optable[i]);
  printf("\n");

  fclose(fp);
  fclose(fp1);
  fclose(fp2);
  fclose(fp3);
  fclose(fp4);
  fclose(fp5);
  fclose(fp6);
  
  return 0;
}
