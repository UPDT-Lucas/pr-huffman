#ifndef HUFFMAN_H
#define HUFFMAN_H
#define MAX 4000

#include "linkedList.h"
#include "pqueue.h"
#include "linkedChar.h"
#include "huffman.h"

Node* createNode(wchar_t data, int freq);
void preOrder(Node* root, char value);
PriorityQueue* createPriorityQueue(int capacity);
Node* construir_arbol_huffman(PriorityQueue* pQueue);
void printHuffmanTree(struct Node* root, int space);
void freeTree(Node* root);
void createTree(wchar_t data[], int freq[], int size);

#endif
