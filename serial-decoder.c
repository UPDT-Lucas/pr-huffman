#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <locale.h>
#include <wchar.h>
#include <dirent.h>
#include <string.h>
#include "huffman.h"
#include <time.h>
#include <sys/time.h>
LinkedList* list;
int translateCounter;
int bitsCounter=0;
char translate[ARRAY_SIZE];

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



void decodeHuffman(FILE* decodeTo, FILE* decodeFrom, Node* root, int numChars) {
    Node* current = root;
    unsigned char bits;
    int nbits = 1;
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

void rebuidFile(char* filename, FILE* fileToRead, int numChars, Node* huffman){
    char decodedFileName[256];
    snprintf(decodedFileName, sizeof(decodedFileName), "%s_decoded.txt", filename);
    FILE *decodedFile = fopen(decodedFileName, "w");
    
    decodeHuffman(decodedFile, fileToRead, huffman, numChars);
    fclose(decodedFile);
}

void leerNombresDeArchivos(FILE *dataE, int nFiles, char* fileNames[], int fileChars[]) {
    int ln, lnc;
    for (int i = 0; i < nFiles; i++) {
        fread(&ln, sizeof(int), 1, dataE);
        fileNames[i] = (char*) malloc(ln);
        fread(fileNames[i], sizeof(char), ln, dataE);
        fread(&lnc, sizeof(int), 1, dataE);
        fileChars[i] = lnc;
    }
}

PriorityQueue* leerArbolHuffman(FILE *dataE) {
    int lenList;
    fread(&lenList, sizeof(int), 1, dataE);

    PriorityQueue* pQueue = createPriorityQueue(1000);
    wchar_t charN;
    int charFreq;
    for (int i = 0; i < lenList; i++) {
        fread(&charN, sizeof(wchar_t), 1, dataE);
        
        fread(&charFreq, sizeof(int), 1, dataE);
        if (charN != L'\0') {
            enqueue(pQueue, createNode(charN, charFreq));
        }
    }
    return pQueue;
}
int main(){

    struct timeval start, end;
    double elapsed_time;
    if (gettimeofday(&start, NULL) != 0) {
        perror("Error getting start time");
        exit(EXIT_FAILURE);
    }
  int nFiles;
  char *locale = setlocale(LC_ALL, "");
  
  FILE *dataE = fopen("textos.bin", "rb");
  char* fileNames[100];
  int fileChars[100];


  fread(&nFiles, sizeof(int), 1, dataE);

  leerNombresDeArchivos(dataE, nFiles, fileNames, fileChars);

  PriorityQueue* pQueue = leerArbolHuffman(dataE);

  Node* arbol_huffman = construir_arbol_huffman(pQueue);

  for (int i = 0; i < nFiles; i++) {
      rebuidFile(fileNames[i], dataE, fileChars[i], arbol_huffman);
  }

  fclose(dataE);
  if (gettimeofday(&end, NULL) != 0) {
        perror("Error getting end time");
        exit(EXIT_FAILURE);
    }

    // Calcular el tiempo transcurrido
    elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    printf("Tiempo transcurrido: %f segundos\n", elapsed_time);
  return 0;
}
