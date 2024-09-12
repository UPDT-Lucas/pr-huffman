#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdlib.h>
#include <wchar.h>

#define ARRAY_SIZE 256 

typedef struct LNode {
    char arr[ARRAY_SIZE];
    wchar_t single_char;
    int code_len;
    int freque;
    struct LNode* next;
} LNode;

typedef struct LinkedList {
    LNode* head;
    LNode* tail;
    LNode* last;
} LinkedList;

LinkedList* create_linked_list();
void insert(LinkedList *list, char* arr, wchar_t single_char,int len,int freq);
void free_list(LinkedList *list);
LNode* createLinkedNode(char*arr,wchar_t single_char,int len,int freq);
char* get_arr_by_char(LinkedList *list, wchar_t single_char);


#endif 
