#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <locale.h>
#include <wchar.h>
#include <dirent.h>
#include <string.h>
#include "huffman.h"


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

int main(){
/*

    wprintf(L"\n");
    for(int i =0 ; i<noAP;i++){
      wprintf(L"%lc,", noAparece[i]);
    }
    int nFiles;
    closedir(dir);
    FILE *dataE = fopen("textos.bin", "rb");
    char* fileNames[100];
    fread(&nFiles,sizeof(int),1,dataE);
    int ln;
    int lnc;
    int fileChars[100];
    //printf("cantidad de archivos %d \n",nFiles);
    for(int i = 0;i<nFiles;i++){
      fread(&ln,sizeof(int),1,dataE);
      //rintf("cantidad de espacio para el nombre  %d \n",ln);
      fileNames[i] = (char*) malloc(ln);
      fread(fileNames[i], sizeof(char), ln, dataE);
      //printf("nombre %s \n",readBuffer);
      fread(&lnc,sizeof(int),1,dataE);
      fileChars[i] = lnc;
      //printf("cantidad letras %d \n",lnc);
    }


    int lenList;
    PriorityQueue* pQueue = createPriorityQueue(1000);
    fread(&lenList,sizeof(int),1,dataE);
    wchar_t charN;
    int charFreq;
    for(int i = 0;i<lenList;i++){
      fread(&charN, sizeof(wchar_t), 1, dataE);
      fread(&charFreq,sizeof(int),1,dataE);
      if(charN !=L'\0'){
        enqueue(pQueue, createNode(charN, charFreq));
      }
      
    }
    Node* arbol_huffman=construir_arbol_huffman(pQueue);

    for(int i = 0;i<nFiles;i++){
      char decodedFileName[256];
      snprintf(decodedFileName, sizeof(decodedFileName), "%s_decoded.txt", fileNames[i]);
      FILE *decodedFile = fopen(decodedFileName, "w");
      decode(decodedFile, dataE, arbol_huffman, fileChars[i]);
      fclose(decodedFile);
   }

    fclose(dataE);
    */
}
