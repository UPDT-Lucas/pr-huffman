#include <stdio.h>
#include <stdlib.h>
#include "pqueue.h"

// Función para intercambiar dos nodos
void swapNodes(Node** a, Node** b) {
    Node* temp = *a;
    *a = *b;
    *b = temp;
}

// Función para aplicar heapify en el min-heap
void minHeapify(PriorityQueue* pQueue, int index) {
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < pQueue->size && pQueue->array[left]->freq < pQueue->array[smallest]->freq) {
        smallest = left;
    }
    if (right < pQueue->size && pQueue->array[right]->freq < pQueue->array[smallest]->freq) {
        smallest = right;
    }
    if (smallest != index) {
        swapNodes(&pQueue->array[smallest], &pQueue->array[index]);
        minHeapify(pQueue, smallest);
    }
}

// Función para extraer el nodo con la frecuencia mínima
Node* dequeue(PriorityQueue* pQueue) {
    if (pQueue->size == 0) {
        return NULL;
    }

    Node* root = pQueue->array[0];
    pQueue->array[0] = pQueue->array[pQueue->size - 1];
    pQueue->size--;
    minHeapify(pQueue, 0);

    return root;
}

// Función para insertar un nodo en el min-heap
void enqueue(PriorityQueue* pQueue, Node* node) {
    pQueue->size++;
    int i = pQueue->size - 1;

    // Insertar el nuevo nodo al final del heap
    pQueue->array[i] = node;

    // Mantener la propiedad del min-heap
    while (i != 0 && pQueue->array[(i - 1) / 2]->freq > pQueue->array[i]->freq) {
        swapNodes(&pQueue->array[i], &pQueue->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}
