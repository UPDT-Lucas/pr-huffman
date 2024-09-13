#ifndef PQUEUE_H
#define PQUEUE_H

#define MAX 4000

typedef struct Node {
  wchar_t data;
  int freq;
  struct Node* left;
  struct Node* right;
} Node;

typedef struct PriorityQueue {
    int size;              // Número actual de elementos en la cola
    int capacity;          // Capacidad máxima de la cola
    Node** array;          // Array de punteros a nodos
} PriorityQueue;

void swapNodes(Node** a, Node** b);
void moveUp(PriorityQueue* p, int index);
void moveDown(PriorityQueue* p, int index);
void enqueue(PriorityQueue* p, Node* node);
Node* dequeue(PriorityQueue* p);
Node* peek(PriorityQueue* p);

#endif
