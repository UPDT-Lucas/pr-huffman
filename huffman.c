#include <stdio.h>
#include <stdlib.h>
#include "pqueue.h"
#include <locale.h>
#include <wchar.h>
#include <dirent.h>
#include "linkedList.h"

wchar_t caracteres[310];
int contadores[310];
int cantidadC=0;
LinkedList* list;
char translate[ARRAY_SIZE];
int translateCounter;

void writeBitsToFile(FILE* file, const char* bits) {
    unsigned char buffer = 0;
    int bitCount = 0;

    for (int i = 0; bits[i] != '\0'; i++) {
        buffer = (buffer << 1) | (bits[i] - '0');  // Añade el bit al buffer
        bitCount++;

        if (bitCount == 8) {  // Si el buffer está lleno, escribir al archivo
            fwrite(&buffer, sizeof(unsigned char), 1, file);
            buffer = 0;
            bitCount = 0;
        }
    }

    // Escribir los bits restantes (si los hay)
    if (bitCount > 0) {
        buffer <<= (8 - bitCount);  // Ajusta los bits restantes
        fwrite(&buffer, sizeof(unsigned char), 1, file);
    }
}


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
    }

    // Si es un nodo hoja, significa que hemos formado el código para un carácter
    if (root->left == NULL && root->right == NULL) {
        translate[translateCounter] = '\0';  // Termina el código actual como una cadena
        insert(list, translate, root->data);  // Inserta el código y el carácter en la lista o tabla de símbolos
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

    // Backtrack: retrocede al nodo anterior
    translateCounter--;
    translate[translateCounter] = '\0';  // Asegúrate de limpiar el código actual para el próximo uso
}

PriorityQueue* createPriorityQueue(int capacity) {
    PriorityQueue* pQueue = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    pQueue->size = 0;
    pQueue->capacity = capacity;
    pQueue->array = (Node**)malloc(capacity * sizeof(Node*));
    return pQueue;
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

void freeTree(Node* root) {
    if (root == NULL) {
        return;
    }

    // Liberar memoria de los subárboles izquierdo y derecho
    freeTree(root->left);
    freeTree(root->right);

    // Liberar la memoria del nodo actual
    free(root);
}

void createTree(wchar_t data[], int freq[], int size) {
    Node *left, *right, *top;

    // Crear la cola de prioridad con capacidad suficiente
    PriorityQueue* pQueue = createPriorityQueue(1000);

    // Insertar todos los caracteres en la cola de prioridad
    for (int i = 0; i < size; i++) {
        enqueue(pQueue, createNode(data[i], freq[i]));
    }

    // Construir el árbol de Huffman
    while (pQueue->size != 1) {
        left = dequeue(pQueue);
        right = dequeue(pQueue);
        top = createNode(L'$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        enqueue(pQueue, top);
    }

    // El nodo restante es el árbol de Huffman completo
    top = dequeue(pQueue);

    // Realizar operaciones con el árbol
    // printHuffmanTree(top, 0);
    preOrder(top, '\0');

    // Liberar memoria
    freeTree(top);
    free(pQueue->array);
    free(pQueue);
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

            FILE *f = fopen(fileToRead, "r,ccs=UTF-8");
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
    /*for(int i =0;i<cantidadC;i++){
      wprintf(L"Caracter:%lc cantidad:%d \n",caracteres[i],contadores[i]);
    }*/
    closedir(dir);
    
    createTree(caracteres,contadores,cantidadC);

    if ((dir = opendir("textos")) == NULL) {
        perror("Cannot open directory");
        return 1;
    }
    FILE *dataC = fopen("textos.bin", "wb");
    wchar_t noAparece[100];
    char bitStream[10000];
    int bitS=0;
    int noAP = 0;
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
              char* codigo = get_arr_by_char(list,ch);
              if(codigo ==NULL){
                  //wprintf(L"%lc \n",ch);
                  int flag = 0;
                  for(int i =0 ; i<noAP;i++){
                      if(noAparece[i]==ch){
                        flag=1;
                        break;
                      }
                  }
                  if(flag==0){
                    noAparece[noAP]=ch;
                    noAP++;
                  }
              }else{
                  for(int i=0;i<1000;i++){  
                    if(codigo[i]!= '\0'){
                      bitStream[bitS]=codigo[i];
                      bitS++;
                    }else{
                      break;
                    }
                  
                  }
                  //fputs(codigo, dataC); // Escribe la cadena en el archivo
                  //fputc(' ', dataC);  // Escribe un salto de línea
              }
                
              
            }
            
            fclose(f);

        }
      }
      writeBitsToFile(dataC,bitStream);
      fclose(dataC);
    wprintf(L"Caracteres no encontrados:");
    for(int i =0 ; i<noAP;i++){
      wprintf(L"%lc,", noAparece[i]);
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
