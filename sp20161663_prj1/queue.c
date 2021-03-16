#include "queue.h"

Node* front = NULL; //  linked-queue의 가장 앞의 원소 가리키는 포인터
Node* rear = NULL;  //  linked-queue의 가장 뒤의 원소 가리키는 포인터
int num = 0;  //  queue의 원소의 개수

Node* alloc_node(char* input) { //  queue의 노드 동적 할당
  Node* node = (Node*)malloc(sizeof(Node));
  strcpy(node->input, input); //  명령어 복사
  node->link = NULL;
  return node;
}

int is_queue_empty() {  //  queue가 비었는지 확인
  return num == 0;
}

int num_of_elms() { //  queue의 원소 개수 반환
  return num;
}

void enqueue(char* input) { //  queue에 삽입
  Node* newNode = alloc_node(input);

  if(is_queue_empty())  //  queue가 빈 상태에서 삽입할 경우
    front = newNode;
  else rear->link = newNode;

  rear = newNode;
  num++;
}

void print_queue() {  //  queue에 저장된 명령어들을 형식에 맞게 출력
  int i = 1;
  Node* cur = front;

  for(; i <= num; i++) {
    printf("        %-3d %s\n", i, cur->input);
    cur = cur->link;
  }
}

void delete_queue() { //  프로그램 종료 전 동적할당된 노드를 해제  
  Node* cur = front;

  while(cur) {
    front = front->link;
    free(cur);
    cur = front;
  }
}
