#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdlib.h>
#include <wchar.h>

#define ARRAY_SIZE 1000 

typedef struct LNode {
    wchar_t arr[ARRAY_SIZE];
    wchar_t single_char;
    struct LNode* next;
} LNode;

typedef struct LinkedList {
    LNode* head;
    LNode* tail;
    LNode* last;
} LinkedList;

LinkedList* create_linked_list();
void insert(LinkedList *list, wchar_t* arr, wchar_t single_char);
void free_list(LinkedList *list);
LNode* createLinkedNode(wchar_t*arr,wchar_t single_char);
wchar_t* get_arr_by_char(LinkedList *list, wchar_t single_char);


#endif 