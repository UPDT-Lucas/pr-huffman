#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <locale.h>
#include <wchar.h>
#include <dirent.h>
#include <string.h>
#include "huffman.h"
#include "globalsE.h"
#include <sys/time.h>
#include "pthread.h"
wchar_t caracteres[310];
int contadores[310];
int cantidadC=0;
LinkedList* list;

wchar_t noAparece[100];
int noAP = 0;
int translateCounter;
int bitsCounter=0;
char translate[ARRAY_SIZE];

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
    fflush(archivo);

    // Recorrer la lista y escribir cada símbolo
    actual = lista->head;
    while (actual != NULL) {
        // Escribir el wchar_t (símbolo)
        fwrite(&actual->single_char, sizeof(wchar_t), 1, archivo);
        fflush(archivo);

        // Escribir la longitud del código
        fwrite(&actual->freque, sizeof(int), 1, archivo);
        fflush(archivo);

        actual = actual->next;
    }
     
    fflush(archivo);
}

int getFileCharsCounts(FILE* file){
    if (file == NULL) {
      perror("Error opening file");
      return 0;
    }
    wchar_t ch;
    int saltos=1;
    int nCarac=0;
    while((ch = fgetwc(file)) != WEOF){
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

typedef struct threadsData {
  char* read;
  char* tname;
} threadsData;

void *writeFileChars(void *arg){
    threadsData * data = (threadsData*)arg;
    LinkedChar listC;
    initLinkedChar(&listC);
    FILE *f = fopen(data-> read, "r");
    FILE *fileTo= fopen(data->tname, "wb");

    if (f == NULL) {
        perror("Error opening f ile");
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
    fclose(f);
    const char delimiter[] = "===END===";
    fwrite(delimiter, sizeof(char), strlen(delimiter), fileTo);
    fclose(fileTo);
    free(data->read);
    free(data->tname);
    free(data);
    return NULL;
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
void append_file_to_destination(FILE *destination_file, const char *source_file_name) {
    FILE *source_file = fopen(source_file_name, "rb");

    if (!source_file) {
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }

    char buffer[1024];  // Buffer to hold file content
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), source_file)) > 0) {
        if (fwrite(buffer, 1, bytes_read, destination_file) != bytes_read) {
            perror("Error writing to destination file");
            fclose(source_file);
            exit(EXIT_FAILURE);
        }
    }

    fclose(source_file);
}

int main(){
    struct timeval start, end;
    long long elapsed_time;
    if (gettimeofday(&start, NULL) != 0) {
        perror("Error getting start time");
        exit(EXIT_FAILURE);
    }


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

    if ((dir = opendir("textos")) == NULL) {
        perror("Cannot open directory");
        return 1;
    }
    pthread_t threads[numD];
    //wprintf(L"en ciclo para recolectar codigos");
    int tc =0;
    while ((dp = readdir(dir))) {

        if (dp->d_type == DT_REG) {
            threadsData* data = (threadsData*)malloc(sizeof(threadsData));
            char fileToRead[256];
            snprintf(fileToRead, sizeof(fileToRead), "textos/%s", dp->d_name);
            data->read = (char*)malloc((strlen(fileToRead) + 1) * sizeof(char));
            strcpy(data->read, fileToRead);
            char fileTemp[256];
            snprintf(fileTemp, sizeof(fileTemp), "%d.bin",tc);
            data->tname = (char*)malloc((strlen(fileTemp) + 1) * sizeof(char));
            strcpy(data->tname, fileTemp);
            pthread_create(&threads[tc], NULL, writeFileChars, (void*)data);

            tc++;   
        }
    }
    for (int i = 0; i < tc; i++) {
        pthread_join(threads[i], NULL);
    }
    fclose(dataC);
    closedir(dir);
    FILE *destination_file = fopen("textos.bin", "ab");
    for (int i = 0; i < tc; i++) {
        char fileTemp[256];
        snprintf(fileTemp, sizeof(fileTemp), "%d.bin",i);
        append_file_to_destination(destination_file,fileTemp);
        remove(fileTemp);
    }
    fclose(destination_file);
    free_list(list);
     if (gettimeofday(&end, NULL) != 0) {
        perror("Error getting end time");
        exit(EXIT_FAILURE);
    }

    // Calcular el tiempo transcurrido
    elapsed_time = (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_usec - start.tv_usec) * 1000LL;    
     printf("Tiempo transcurrido: %lld nanosegundos\n", elapsed_time);

    return 0;
}


