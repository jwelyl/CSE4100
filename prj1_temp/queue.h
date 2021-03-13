#ifndef __QUEUE__
#define __QUEUE__

#include "header.h"

typedef struct node {
  char input[INPUT_LEN];
  struct node* link;
} Node;

Node* alloc_node(char* input);
int isQueueEmpty();
int numOfElms();
void enqueue(char* input);
void printQueue();
void deleteQueue();

#endif

