#ifndef __QUEUE__
#define __QUEUE__

#include "header.h"

typedef struct node {
  char cmd[CMD_LEN];
  struct node* link;
} Node;

Node* alloc_node(char* cmd);
int isQueueEmpty();
int numOfElms();
void enqueue(char* cmd);
void printQueue();
void deleteQueue();

#endif

