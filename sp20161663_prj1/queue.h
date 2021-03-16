#ifndef __QUEUE__
#define __QUEUE__

#include "20161663.h"

typedef struct node { //  history queue에 저장될 명령어를 위한 노드 구조체
  char input[INPUT_LEN];
  struct node* link;
} Node;

Node* alloc_node(char* input);
int is_queue_empty();
int num_of_elms();
void enqueue(char* input);
void print_queue();
void delete_queue();

#endif

