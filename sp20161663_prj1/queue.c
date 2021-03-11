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

int isQueueEmpty() {
  return num == 0;
}

int numOfElms() {
  return num;
}

void enqueue(char* input) {
  Node* newNode = alloc_node(input);

  if(isQueueEmpty())  //  queue가 빈 상태에서 삽입할 경우
    front = newNode;
  else rear->link = newNode;

  rear = newNode;
  num++;
}

void printQueue() {
  int i = 1;
  Node* cur = front;

  for(; i <= num; i++) {
    printf("%-3d %s\n", i, cur->input);
    cur = cur->link;
  }
}

void deleteQueue() {
  Node* cur = front;

  while(cur) {
    front = front->link;
    free(cur);
    cur = front;
  }
}
