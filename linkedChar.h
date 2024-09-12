#ifndef linkedChar_H
#define linkedChar_H_H


typedef struct CNode {
    char data;             // Puntero a los datos (subcadena de caracteres)
    struct CNode *next;      // Puntero al siguiente nodo
} CNode;

typedef struct LinkedChar {
    CNode *head;             // Puntero al nodo cabeza (inicio de la lista)
    CNode *tail;             // Puntero al nodo cola (final de la lista)
} LinkedChar;


CNode* createNodeC(const char data);
void initLinkedChar(LinkedChar *list);
void insertChar(LinkedChar *list, const char data);
void freeNodeChar(CNode *node);
void freeLinkedchar(LinkedChar *list);
void printLinkedChar(const LinkedChar *list);

#endif