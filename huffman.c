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
#define INPUT_SIZE 32

wchar_t caracteres[310];
int contadores[310];
int cantidadC=0;
LinkedList* list;
char translate[ARRAY_SIZE];
int translateCounter;
LinkedChar listC;
int bitsCounter=0;
wchar_t noAparece[100];
int noAP = 0;

void writeBitsToFile(FILE* file, const LinkedChar* list) {
    unsigned char buffer = 0;  
    int bitCount = 0;          

    CNode *current = list->head;
    while (current != NULL) {
      
        buffer = (buffer << 1) | (current->data - '0');  
        bitCount++;

       
        if (bitCount == 8) {
            fwrite(&buffer, sizeof(unsigned char), 1, file);
            buffer = 0;       
            bitCount = 0;    
        }

        current = current->next;  
    }

    if (bitCount > 0) {
        buffer <<= (8 - bitCount); 
        fwrite(&buffer, sizeof(unsigned char), 1, file);
    }
}

void escribir_encabezado(FILE* archivo, LinkedList* lista) {
    int numero_simbolos = 0;
    LNode* actual = lista->head;

    // Calcular el número de símbolos
    while (actual != NULL) {
        numero_simbolos++;
        actual = actual->next;
    }

    // Escribir el número de símbolos
    fwrite(&numero_simbolos, sizeof(int), 1, archivo);

    // Recorrer la lista y escribir cada símbolo
    actual = lista->head;
    while (actual != NULL) {
        // Escribir el wchar_t (símbolo)
        fwrite(&actual->single_char, sizeof(wchar_t), 1, archivo);

        // Escribir la longitud del código
        fwrite(&actual->freque, sizeof(int), 1, archivo);

        actual = actual->next;
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

void createTree(wchar_t data[], int freq[], int size) {
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

void decode(FILE* decodeTo, FILE* decodeFrom, Node* root, int numChars) {
    Node* current = root;
    unsigned char bits;
    int nbits = 0;
    int charsDecoded = 0;

    while (charsDecoded < numChars && fread(&bits, sizeof(unsigned char), 1, decodeFrom) == 1) {  // Leer un byte a la vez
        nbits = 8;
        while (nbits > 0 && charsDecoded < numChars) {
            nbits--;
            
            if (bits & (1 << nbits)) { 
                current = current->right;
            } else { 
                current = current->left;
            }

            if (current->left == NULL && current->right == NULL) {
                fputwc(current->data, decodeTo); 
                current = root;
                charsDecoded++;
            }
        }
    }
}

int countChars(FILE* f){
    if (f == NULL) {
        perror("Error opening file");
    }
    wchar_t ch;
    int saltos =1;
    int nCarac=0;
    while((ch = fgetwc(f)) != WEOF){
        if(ch == L'\n'){
            if(saltos<1){
            nCarac++;
            inListCar(ch);
            }
            saltos--;
        }else if(ch!=L'\0'){
        nCarac++;
        inListCar(ch);
        }
    }
    return nCarac;
}

int writeFileChars(FILE* f, FILE* fileTo){
    if (f == NULL) {
        perror("Error opening file");
    }
    wchar_t ch;
    int bitS=0;
    while((ch = fgetwc(f)) != WEOF){
        char* codigo = get_arr_by_char(list,ch);
        if(codigo ==NULL){
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
            if(codigo[i]!= '\0'&&bitS<8){

                insertChar(&listC,codigo[i]);
                bitS++;

            }else if(codigo[i]!= '\0'&&bitS>=8){
                writeBitsToFile(fileTo,&listC);
                freeLinkedchar(&listC); 
                bitS=0;
                insertChar(&listC,codigo[i]);
                bitS++;
            }else if(codigo[i]== '\0'&&bitS>=8){
                writeBitsToFile(fileTo,&listC);
                freeLinkedchar(&listC); 
                bitS=0;
            }else{
                break;
            }
            
            }
        }
    }
    return noAP;
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
    
    int numD=0;

    while ((dp = readdir(dir))) {
      if (dp->d_type == DT_REG) {
          numD++;
      }
    }
    closedir(dir);

    FILE *dataC = fopen("textos.bin", "wb");
    fwrite(&numD, sizeof(int), 1, dataC);
    if ((dir = opendir("textos")) == NULL) {
        perror("Cannot open directory");
        return 1;
    }
    //printf("ciclo para recolectar apariciones\n");
    while ((dp = readdir(dir))) {
        if (dp->d_type == DT_REG) {
            char fileToRead[256];
            snprintf(fileToRead, sizeof(fileToRead), "textos/%s", dp->d_name);

            FILE *f = fopen(fileToRead, "r,ccs=UTF-8");
            int nCarac = countChars(f);

            int lenF=strlen(fileToRead)+1;
            fwrite(&lenF, sizeof(int), 1, dataC);
            fwrite(fileToRead, sizeof(char), strlen(fileToRead) + 1, dataC);
            fwrite(&nCarac, sizeof(int), 1, dataC);
            
            fclose(f);
        }
    }
    closedir(dir);

    createTree(caracteres,contadores,cantidadC);
    escribir_encabezado(dataC,list);
    initLinkedChar(&listC);

    if ((dir = opendir("textos")) == NULL) {
        perror("Cannot open directory");
        return 1;
    }
    
    //wprintf(L"en ciclo para recolectar codigos");
    while ((dp = readdir(dir))) {

        if (dp->d_type == DT_REG) {
            char fileToRead[256];
            snprintf(fileToRead, sizeof(fileToRead), "textos/%s", dp->d_name);

            FILE *f = fopen(fileToRead, "r");
            noAP += writeFileChars(f, dataC);
            fclose(f);
        }
      }
    fclose(dataC);
    free_list(list);
    return 0;

}
