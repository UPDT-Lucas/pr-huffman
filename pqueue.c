#include <stdio.h>
#include <stdlib.h>
#include "pqueue.h"

// Intercambia dos nodos
void swap(Node** a, Node** b) {
  Node* temp = *a;
  *a = *b;
  *b = temp;
}

// Mueve un nodo hacia arriba en el heap para mantener la propiedad del min-heap
void moveUp(PriorityQueue* p, int index) {
  // Mientras no estés en la raíz y el nodo actual tenga una frecuencia menor que su padre
  if (index && p->items[(index - 1) / 2]->freq > p->items[index]->freq) {
    // Intercambia el nodo con su padre
    swap(&p->items[(index - 1) / 2], &p->items[index]);
    // Recursivamente mueve el nodo hacia arriba
    moveUp(p, (index - 1) / 2);
  }
}

// Mueve un nodo hacia abajo en el heap para mantener la propiedad del min-heap
void moveDown(PriorityQueue* p, int index) {
  int smallest = index;
  int left = 2 * index + 1;
  int right = 2 * index + 2;

  // Encuentra el hijo con la frecuencia mínima
  if (left < p->size && p->items[left]->freq < p->items[smallest]->freq)
    smallest = left;
  
  if (right < p->size && p->items[right]->freq < p->items[smallest]->freq)
    smallest = right;

  // Si el nodo actual no es el más pequeño, intercámbialo con el más pequeño y repite
  if (smallest != index) {
    swap(&p->items[index], &p->items[smallest]);
    moveDown(p, smallest);
  }
}

// Inserta un nuevo nodo en la cola de prioridad
void enqueue(PriorityQueue* p, Node* node) {
  if (p->size == MAX) {
    printf("Queue is full\n");
    return;
  }
  p->items[p->size] = node;
  moveUp(p, p->size);
  p->size++;
}

// Elimina y retorna el nodo de menor frecuencia de la cola de prioridad
Node* dequeue(PriorityQueue* p) {
  if (p->size == 0) {
    printf("Queue is empty\n");
    return NULL;
  }
  Node* item = p->items[0];
  p->items[0] = p->items[--p->size];
  moveDown(p, 0);
  return item;
}

// Retorna el nodo de menor frecuencia sin eliminarlo
Node* peek(PriorityQueue* p) {
  if (p->size == 0) {
    printf("Queue is empty\n");
    return NULL;
  }
  return p->items[0];
}
