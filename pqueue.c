#include <stdio.h>
#include <stdlib.h>
#include "pqueue.h"

void swap(Node** a, Node** b){
  Node* temp = *a;
  *a = *b;
  *b = temp;
}

void moveUp(PriorityQueue* p, int index){
  if(index && p->items[(index-1) / 2] > p->items[index]){
    swap(&p->items[(index-1) / 2], &p->items[index]);
    moveUp(p, (index-1) / 2);
  }
}

void  moveDown(PriorityQueue* p, int index){
  int smallest = index;
  int left = 2 * index + 1;
  int right = 2 * index + 2;
  
  if(left < p->size && 
    p->items[left] < p->items[smallest])
    smallest = left;
      
  if(right < p->size && 
    p->items[right] < p->items[smallest])
    smallest = right;

  if (smallest != index) {
    swap(&p->items[index], &p->items[smallest]);
    moveDown(p, smallest);
  }
}

void enqueue(PriorityQueue* p, Node* node){
  if(p->size == MAX){
    printf("Queue is full\n");
    return;
  }
  p->items[p->size++] = node;
  moveUp(p, p->size-1);
}

Node* dequeue(PriorityQueue* p){
  if(!p->size){
    printf("Queue is empty\n");
  }
  Node* item = p->items[0];
  p->items[0] = p->items[--p->size];
  moveDown(p, 0);
  return item;
}

Node* peek(PriorityQueue* p){
  if(!p->size){
    printf("Queue is empty\n");
  }
  return p->items[0];
}

