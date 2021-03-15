#include "queue.h"

Node* front = NULL;
Node* rear = NULL;
int num = 0;

Node* alloc_node(char* input) {
  Node* node = (Node*)malloc(sizeof(Node));
  strcpy(node->input, input);
  node->link = NULL;
  return node;
}

int is_queue_empty() {
  return num == 0;
}

int num_of_elms() {
  return num;
}

void enqueue(char* input) {
  Node* newNode = alloc_node(input);

  if(is_queue_empty())  //  queue가 빈 상태에서 삽입할 경우
    front = newNode;
  else rear->link = newNode;

  rear = newNode;
  num++;
}

void print_queue() {
  int i = 1;
  Node* cur = front;

  for(; i <= num; i++) {
    printf("        %-3d %s\n", i, cur->input);
    cur = cur->link;
  }
}

void delete_queue() {
  Node* cur = front;

  while(cur) {
    front = front->link;
    free(cur);
    cur = front;
  }
}
