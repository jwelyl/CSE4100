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

/* Proj1 */
/* 1MB 메모리 공간 크기*/
#define MAX_MEM_SIZE 1048576
#define INPUT_LEN        100
#define MAX_CMD           11
#define MAX_OPT           11

/* opcode, opcodelist 명령어에 사용할 배열 크기 */
#define MNEMONIC           7
#define OPCODE             3
#define OPCODE_INPUT      30
#define OPTABLE_SIZE      20

/* format 저장 추가 */
#define FORMAT             4

/* dump 명령어에 사용할 배열 크기 */
#define ADDRESS_COL        6
#define HEX_COL            3

/* Proj2 */
/* filename 크기 */
#define FILENAME          30
#define PROGRAM_NAME       7

/* symbol table 크기*/
#define SYMTAB_SIZE       13

/* LABLE, OPCODE, OPERAND 크기 */
#define STRING_SIZE       30

/* LOCCTR 크기 */
#define LOCCTR_SIZE        5

/* Header, Text, Modification, End record */
#define HEADER             20
#define TEXT               70
#define MODIFICATION       10
#define END                8

/* object code */
//#define OBJ_BIN            33 //  2진수일때 최대 길이(4형식)
#define OBJ_HEX            9  //  16진수일때 ''

/* register mnemonic */
#define A                  0
#define X                  1
#define L                  2
#define B                  3
#define S                  4
#define T                  5
#define F                  6
#define PC                 8
#define SW                 9

/* Proj3 */
#define SYMBOL_SIZE        7    //  SYMBOL 이름, 주소 크기(6)
#define STMBOL_REFN        3    //  SYMBOL REFERENCE NUMBER 크기(3)
#define ESTAB_SIZE        13    //  최대 ESTAB SIZE(Hash Table) 
#define REFERENCE_N      100    //  최대 reference 개수

#endif
