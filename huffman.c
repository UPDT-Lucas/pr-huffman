#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <locale.h>
#include <wchar.h>
#include <dirent.h>
#include  <string.h>
#include "linkedList.h"
#include "pqueue.h"
#include "linkedChar.h"

#include "globalsE.h"
#define INPUT_SIZE 32

Node* createNode(wchar_t data, int freq){
  Node* node = (Node*)malloc(sizeof(Node));
  node->data = data;
  node->freq = freq;
  node->left = node->right = NULL;
  return node;
}

void preOrder(Node* root, char value) {
    if (root == NULL) return;  // Verificación adicional por seguridad

    // Si el valor no es '\0', lo agregamos al código actual
    if (value != '\0') {
        translate[translateCounter] = value;
        translateCounter++;
        bitsCounter++;
    }

    if (root->left == NULL && root->right == NULL) {
        translate[translateCounter] = '\0';  
        insert(list, translate, root->data,bitsCounter,root->freq);  
    } else {
        // Recorremos el subárbol izquierdo, agregando '0' al código
        if (root->left != NULL) {
            preOrder(root->left, '0');
        }
        // Recorremos el subárbol derecho, agregando '1' al código
        if (root->right != NULL) {
            preOrder(root->right, '1');
        }
    }

    translateCounter--;
    bitsCounter--;
    translate[translateCounter] = '\0';  
}
PriorityQueue* createPriorityQueue(int capacity) {
    PriorityQueue* pQueue = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    pQueue->size = 0;
    pQueue->capacity = capacity;
    pQueue->array = (Node**)malloc(capacity * sizeof(Node*));
    return pQueue;
}

Node* construir_arbol_huffman(PriorityQueue* pQueue) {
    while (pQueue->size > 1) {
        // Extraer los dos nodos con la frecuencia más baja
        Node* izquierda = dequeue(pQueue);
        Node* derecha = dequeue(pQueue);

        // Crear un nuevo nodo con estos dos nodos como hijos
        Node* nuevo_nodo = createNode('\0', izquierda->freq + derecha->freq);
        nuevo_nodo->left = izquierda;
        nuevo_nodo->right = derecha;

        // Insertar el nuevo nodo en la cola de prioridad
        enqueue(pQueue, nuevo_nodo);
    }

    // El único nodo restante es la raíz del árbol de Huffman
    return dequeue(pQueue);
}


void printHuffmanTree(struct Node* root, int space) {
    if (root == NULL) return;
    
    int COUNT = 5;
    space += COUNT;

    printHuffmanTree(root->right, space);

    wprintf(L"\n");
    for (int i = COUNT; i < space; i++)
        printf(" ");
    if (root->data)
        wprintf(L"[%lc:%d]", root->data, root->freq);
    else
        wprintf(L"(%d)", root->freq);

    
    printHuffmanTree(root->left, space);
}

void freeTree(Node* root) {
    if (root == NULL) {
        return;
    }
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

void createTree(wchar_t data[], int freq[], int size, LinkedList* list) {
    Node *left, *right, *top;
    
   
    PriorityQueue* pQueue = createPriorityQueue(1000);

    
    for (int i = 0; i < size; i++) {
        enqueue(pQueue, createNode(data[i], freq[i]));
    }

    while (pQueue->size != 1) {
    
        left = dequeue(pQueue);
        right = dequeue(pQueue);
        top = createNode(L'$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        enqueue(pQueue, top);
        
    }
    top = dequeue(pQueue);
    // Realizar operaciones con el árbol
    //printHuffmanTree(top, 0);
    preOrder(top, '\0');

    // Liberar memoria
    freeTree(top);
    free(pQueue->array);
    free(pQueue);
}
