#include <stdio.h>
#include <stdlib.h>
#include "pqueue.h"
#include <locale.h>
char caracteres[200];
int contadores[200];
int cantidadC=0;

Node* createNode(char data, int freq){
  Node* node = (Node*)malloc(sizeof(Node));
  node->data = data;
  node->freq = freq;
  node->left = node->right = NULL;
  return node;
}

void createTree(char data[], int freq[], int size){
  Node *left, *right, *top;
  PriorityQueue* p = (PriorityQueue*)malloc(sizeof(PriorityQueue));
  p->size = 0;
 
  for(int i = 0; i < size; i++)
    enqueue(p, createNode(data[i], freq[i]));

  while(p->size != 1){
    left = dequeue(p);
    right = dequeue(p);
    top = createNode('$', left->freq + right->freq);
    top->left = left;
    top->right = right;
    enqueue(p, top);
  }
}
int inListCar(char val){
    for(int i = 0;i<cantidadC;i++){
      if(val == caracteres[i]){
        contadores[i] +=1;
        return 1;
      }
    }
    caracteres[cantidadC]=val;
    contadores[cantidadC]=1;
    cantidadC++;
    return 0;

}

int main(){
  FILE *f = fopen("test1.txt", "r");
  setlocale(LC_CTYPE, "");
  if(NULL == f){
    printf("File not found\n");
    return 1;
  }

  char ch;


  while((ch = fgetc(f)) != EOF){
    inListCar(ch);
    
  }

  fclose(f);
  
  for(int i =0;i<cantidadC;i++){
    printf("Caracter: %c cantidad: %d \n",caracteres[i],contadores[i]);
  }
  /*PriorityQueue p;
  p.size = 0;
  enqueue(&p, 3);
  enqueue(&p, 2);
  enqueue(&p, 1);
  printf("%d\n", dequeue(&p));
  printf("peeking: %d\n", peek(&p));
  printf("%d\n", dequeue(&p));
  printf("%d\n", dequeue(&p));
  printf("%d\n", dequeue(&p));
  */
  
  return 0;
}
