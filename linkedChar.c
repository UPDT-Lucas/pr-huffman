#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedChar.h"

CNode* createNodeC(const char data) {
    CNode *newNode = (CNode*) malloc(sizeof(CNode));
    if (!newNode) {
        fprintf(stderr, "Error al asignar memoria para un nuevo nodo\n");
        exit(EXIT_FAILURE);
    }
    newNode->data = data;  // Copiar los datos a una nueva cadena
    newNode->next = NULL;
    return newNode;
}

// Función para inicializar la lista enlazada
void initLinkedChar(LinkedChar *list) {
    list->head = NULL;
    list->tail = NULL;
}

// Función para insertar un nodo al final de la lista enlazada
void insertChar(LinkedChar *list, const char data) {
    CNode *newNode = createNodeC(data);
    
    if (list->head == NULL) {  // Lista vacía
        list->head = newNode;
        list->tail = newNode;
    } else {
        list->tail->next = newNode;
        list->tail = newNode;
    }
}

// Función para liberar un nodo específico de la lista
void freeNodeChar(CNode *node) {
    if (node) {
        free(node);  // Liberar la memoria del nodo
    }
}

// Función para liberar toda la lista enlazada
void freeLinkedchar(LinkedChar *list) {
    CNode *current = list->head;
    CNode *next;
    
    while (current != NULL) {
        next = current->next;
        freeNodeChar(current);
        current = next;
    }
    
    list->head = NULL;
    list->tail = NULL;
}

// Función de ejemplo para imprimir la lista enlazada
void printLinkedChar(const LinkedChar *list) {
    CNode *current = list->head;
    while (current != NULL) {
        printf("%s", current->data);
        current = current->next;
    }
    printf("\n");
}