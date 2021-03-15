#ifndef __HEADER__
#define __HEADER__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define FALSE              0
#define TRUE               1
#define NONE              -1

/* 1MB 메모리 공간 크기*/
#define MAX_MEM_SIZE 1048576
#define INPUT_LEN        100
#define MAX_CMD           11
#define MAX_OPT           11

/* opcode, opcodelist 명령어에 사용할 배열 크기 명령*/
#define MNEMONIC           7
#define OPCODE             3
#define OPCODE_INPUT      30
#define OPTABLE_SIZE      20

/* dump 명령어에 사용할 배열 크기 */
#define ADDRESS_COL        6
#define HEX_COL            3 

#endif
