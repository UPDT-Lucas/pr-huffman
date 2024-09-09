#include <stdio.h>
#include <stdlib.h>
#include "pqueue.h"
#include <locale.h>
#include <wchar.h>
#include <dirent.h>
#include "linkedList.h"

wchar_t caracteres[200];
int contadores[200];
int cantidadC=0;
LinkedList* list;
wchar_t translate[ARRAY_SIZE];
int translateCounter;




Node* createNode(wchar_t data, int freq){
  Node* node = (Node*)malloc(sizeof(Node));
  node->data = data;
  node->freq = freq;
  node->left = node->right = NULL;
  return node;
}

void preOrder(Node* root,wchar_t value){

  if(value!=L'\0'){
    translate[translateCounter]=value;
    translateCounter++;

  }
  if(root->left==NULL && root->right==NULL){
    insert(list, translate,root->data);
    
  }
  if(root->left!=NULL){
    preOrder(root->left,L'0');
  }
  if(root->right!=NULL){
    preOrder(root->right,L'1');
  }
  translateCounter--;
  translate[translateCounter]='\0';
  return;
}

void printHuffmanTree(struct Node* root, int space) {
    if (root == NULL) return;
    
    // Define el espaciado entre niveles
    int COUNT = 5;
    space += COUNT;

    // Imprime el subárbol derecho primero
    printHuffmanTree(root->right, space);

    // Imprime el nodo actual después de los espacios
    wprintf(L"\n");
    for (int i = COUNT; i < space; i++)
        printf(" ");
    if (root->data)
        wprintf(L"[%lc:%d]", root->data, root->freq);
    else
        wprintf(L"(%d)", root->freq);

    // Imprime el subárbol izquierdo
    printHuffmanTree(root->left, space);
}

void createTree(wchar_t data[], int freq[], int size) {
  Node *left, *right, *top;

  PriorityQueue* p = (PriorityQueue*)malloc(sizeof(PriorityQueue));
  p->size = 0;
  for (int i = 0; i < size; i++) {
    enqueue(p, createNode(data[i], freq[i]));
  }

  while (p->size != 1) {
    left = dequeue(p);
    right = dequeue(p);
    top = createNode(L'$', left->freq + right->freq);
    top->left = left;
    top->right = right;
    enqueue(p, top);
  }

  //printHuffmanTree(top, 0);
  preOrder(top, L'\0');
}
int inListCar(wchar_t val){
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
    list = create_linked_list();
    char *locale = setlocale(LC_ALL, "");
    DIR *dir;
    struct dirent *dp;
    // Abre el directorio "textos"
    if ((dir = opendir("textos")) == NULL) {
        perror("Cannot open directory");
        return 1;
    }

    while ((dp = readdir(dir))) {

        if (dp->d_type == DT_REG) {
            

            char fileToRead[256];
            snprintf(fileToRead, sizeof(fileToRead), "textos/%s", dp->d_name);

            FILE *f = fopen(fileToRead, "r");
            if (f == NULL) {
                perror("Error opening file");
                continue;
            }
            wchar_t ch;
            int saltos =1;
            while((ch = fgetwc(f)) != WEOF){
              if(ch == L'\n'){
                  if(saltos<1){
                    inListCar(ch);
                  }
                  saltos--;
              }else if(ch!=L'\0'){

                
                inListCar(ch);

              }
              
              
            }
            
            fclose(f);
        }
    }
    for(int i =0;i<cantidadC;i++){
      wprintf(L"Caracter:%lc cantidad:%d \n",caracteres[i],contadores[i]);
    }
    closedir(dir);

    createTree(caracteres,contadores,cantidadC);

    if ((dir = opendir("textos")) == NULL) {
        perror("Cannot open directory");
        return 1;
    }
    FILE *dataC = fopen("textos.bin", "w");
    while ((dp = readdir(dir))) {

        if (dp->d_type == DT_REG) {
            

            char fileToRead[256];
            snprintf(fileToRead, sizeof(fileToRead), "textos/%s", dp->d_name);

            FILE *f = fopen(fileToRead, "r");
            if (f == NULL) {
                perror("Error opening file");
                continue;
            }
            wchar_t ch;
            
            while((ch = fgetwc(f)) != WEOF){
              wchar_t* codigo = get_arr_by_char(list,ch);
              if(codigo ==NULL){
                  wprintf(L"No existe codigo para este caracter: %lc",ch);
              }else{
                  fputws(codigo, dataC); // Escribe la cadena en el archivo
                  fputwc(L' ', dataC);  // Escribe un salto de línea
              }
                
              
            }
            
            fclose(f);

        }
      }
      fclose(dataC);
    
    
    LNode* current = list->head->next;  // Saltamos el nodo de la cabeza
    while (current != list->tail) {  // Iteramos hasta el nodo de la cola
        wprintf(L"Contenido del nodo: %ls, Caracter único: %lc\n", current->arr, current->single_char);
        current = current->next;
    }
    closedir(dir);
    


    free_list(list);
  return 0;
  
  /*LinkedList* list = create_linked_list();
    if (list == NULL) {
        wprintf(L"Error al crear la lista enlazada.\n");
        return 1;
    }

    // Insertar algunos nodos en la lista
    wchar_t arr1[ARRAY_SIZE] = L"Primer nodo";
    wchar_t arr2[ARRAY_SIZE] = L"Segundo nodo";
    wchar_t arr3[ARRAY_SIZE] = L"Tercer nodo";

    insert(list, arr1, L'A');
    insert(list, arr2, L'B');
    insert(list, arr3, L'C');

    // Recorrer la lista e imprimir los elementos
    LNode* current = list->head->next;  // Saltamos el nodo de la cabeza
    while (current != list->tail) {  // Iteramos hasta el nodo de la cola
        wprintf(L"Contenido del nodo: %ls, Caracter único: %lc\n", current->arr, current->single_char);
        current = current->next;
    }

    // Liberar la memoria de la lista
    free_list(list);
    */
   
}
