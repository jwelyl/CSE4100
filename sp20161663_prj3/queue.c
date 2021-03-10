#include "queue.h"

Node* front = NULL;
Node* rear = NULL;
int num = 0;

Node* alloc_node(char* cmd) {
  Node* node = (Node*)malloc(sizeof(Node));
  strcpy(node->cmd, cmd);
  node->link = NULL;
  return node;
}

int isQueueEmpty() {
  return num == 0;
}

int numOfElms() {
  return num;
}

void enqueue(char* cmd) {
  Node* newNode = alloc_node(cmd);

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
    printf("%-3d %s\n", i, cur->cmd);
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
