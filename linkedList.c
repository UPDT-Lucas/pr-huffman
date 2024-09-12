#include <stdio.h>
#include <stdlib.h>
#include "linkedList.h"
#include <string.h>

void insert(LinkedList *list,char* arr, wchar_t single_char,int len,int freq){
	LNode* node = createLinkedNode(arr,single_char,len,freq);
	if(list->head->next==list->tail){
		list->head->next = node;
		node->next=list->tail;
		list->last=node;
		return;
	}
	list->last->next=node;
	node->next=list->tail;
	list->last = node;
}
LNode* createLinkedNode(char*arr,wchar_t single_char,int len,int freq){

	LNode* new_node = (LNode*)malloc(sizeof(LNode));
	if(new_node==NULL){
		return NULL;
	}
	strncpy(new_node->arr, arr, ARRAY_SIZE - 1);
	new_node->arr[ARRAY_SIZE - 1] = L'\0';
	new_node->single_char = single_char;
	new_node->next = NULL;
	new_node->code_len=len;
    new_node->freque=freq;	
    return new_node;
}

LinkedList* create_linked_list() {

    LinkedList* list = (LinkedList*)malloc(sizeof(LinkedList));
    if (list == NULL) {
        return NULL;
    }

    list->head = (LNode*)malloc(sizeof(LNode));
    list->tail = (LNode*)malloc(sizeof(LNode));

    if (list->head == NULL || list->tail == NULL) {
        free(list->head);
        free(list->tail);
        free(list);
        return NULL;
    }

    list->head->next = list->tail;
    list->tail->next = NULL;
    list->last = list->head;

    return list;

}

void free_list(LinkedList *list) {
    LNode* current = list->head;
    LNode* next_node;

    while (current != NULL) {
        next_node = current->next;
        free(current);
        current = next_node;
    }

    free(list);
}
char* get_arr_by_char(LinkedList *list, wchar_t single_char) {
    if (list == NULL) {
        return NULL; // Lista no inicializada
    }

    LNode* current = list->head->next;
    while (current != list->tail) {
        if (current->single_char == single_char) {
            return current->arr;
        }
        current = current->next;
    }

    return NULL; // Carácter no encontrado
}
