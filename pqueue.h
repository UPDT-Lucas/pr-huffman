#ifndef PRIOQUEUE_H
#define PRIOQUEUE_H

#define MAX 1000

typedef struct Node {
  wchar_t data;
  int freq;
  struct Node* left;
  struct Node* right;
} Node;

typedef struct {
    Node* items[MAX];
    int size;
} PriorityQueue;

void swap(Node** a, Node** b);
void moveUp(PriorityQueue* p, int index);
void moveDown(PriorityQueue* p, int index);
void enqueue(PriorityQueue* p, Node* node);
Node* dequeue(PriorityQueue* p);
Node* peek(PriorityQueue* p);

#endif
