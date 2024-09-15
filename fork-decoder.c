#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <locale.h>
#include <wchar.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include "huffman.h"
#include <sys/time.h>
#define DELIMITER "===END==="
#define DELIM_LEN (sizeof(DELIMITER) - 1)

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

            // Si es una hoja, decodificamos un carácter
            if (current->left == NULL && current->right == NULL) {
                fputwc(current->data, decodeTo); 
                current = root;
                charsDecoded++;

                // Verificar si se ha alcanzado el número máximo de caracteres
                if (charsDecoded >= numChars) {
                    return;
                }
            }
        }
    }
}

void rebuidFile(char* filename, FILE* fileToRead, int numChars, Node* huffman){
    char decodedFileName[256];
    snprintf(decodedFileName, sizeof(decodedFileName), "decoded/%s_decoded.txt", filename);
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

void createDirectory(char* name){
    int status = mkdir(name, 0777); 

    if (status == 0) {
        printf("Directorio '%s' creado exitosamente.\n", name);
    } else {
        if (errno == EEXIST) {
            printf("El directorio '%s' ya existe.\n", name);
        } else {
            perror("Error al crear el directorio");
        }
    }
}


int eliminarArchivo(const char *path) {
    if (unlink(path) == -1) {
        perror("Error al eliminar archivo");
        return -1;
    }
    return 0;
}

int eliminarDirectorio(const char *path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("Error al abrir directorio");
        return -1;
    }

    struct dirent *entry;
    char filepath[1024];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(filepath, sizeof(filepath), "%s/%s", path, entry->d_name);

        if (entry->d_type == DT_REG) {
            if (eliminarArchivo(filepath) != 0) {
                closedir(dir);
                return -1;
            }
        }
        else if (entry->d_type == DT_DIR) {
            if (eliminarDirectorio(filepath) != 0) {
                closedir(dir);
                return -1;
            }
        }
    }

    closedir(dir);

    if (rmdir(path) == -1) {
        perror("Error al eliminar directorio");
        return -1;
    }

    return 0;
}

void obtener_nombre_archivo(const char* ruta, char* nombre) {
    // Encontrar el último '/' para obtener el nombre del archivo
    const char *archivo = strrchr(ruta, '/');
    if (archivo) {
        archivo++;  // Saltar el '/'
    } else {
        archivo = ruta;  // Si no hay '/', el archivo es la ruta completa
    }

    // Copiar el nombre del archivo sin la extensión
    strncpy(nombre, archivo, strrchr(archivo, '.') - archivo);
    nombre[strrchr(archivo, '.') - archivo] = '\0';  // Asegurar el terminador nulo
}

void separateChunks(FILE* inputFile, char* fileNames[], int nFiles) {
    char c;
    size_t delimiterIndex = 0;
    int counter = 0;

    // Crear la carpeta temporal si no existe
    createDirectory("tmp");

    while (counter < nFiles && fread(&c, sizeof(char), 1, inputFile) == 1) {
        // Generar el nombre base del archivo de chunk usando obtener_nombre_archivo
        char chunkFileBaseName[256];
        obtener_nombre_archivo(fileNames[counter], chunkFileBaseName);

        // Crear el nombre final para el archivo del chunk
        char chunkFileName[256];
        snprintf(chunkFileName, sizeof(chunkFileName), "tmp/%s.txt", chunkFileBaseName);

        // Abrir el archivo de chunk
        FILE* chunkFile = fopen(chunkFileName, "wb");
        if (chunkFile == NULL) {
            perror("Error al abrir archivo para el chunk");
            return;
        }

        // Escribir en el archivo mientras se lea el contenido
        while (fread(&c, sizeof(char), 1, inputFile) == 1) {
            fwrite(&c, sizeof(char), 1, chunkFile);

            // Verificar si llegamos al delimitador
            if (c == DELIMITER[delimiterIndex]) {
                delimiterIndex++;
            } else {
                delimiterIndex = (c == DELIMITER[0]) ? 1 : 0;
            }

            // Cuando encontramos el delimitador completo, cerramos el chunk y pasamos al siguiente archivo
            if (delimiterIndex == DELIM_LEN) {
                delimiterIndex = 0;
                break;
            }
        }

        fclose(chunkFile);  // Cerrar el archivo actual del chunk
        counter++;
    }
}


int main(){
    struct timeval start, end;
    double elapsed_time;
    if (gettimeofday(&start, NULL) != 0) {
        perror("Error getting start time");
        exit(EXIT_FAILURE);
    }
    
    int nFiles;
    FILE *dataE = fopen("textos.bin", "rb");
    if (dataE == NULL) {
        perror("Error opening input file");
        return 1;
    }
    char* fileNames[100];
    int fileChars[100];

    fread(&nFiles, sizeof(int), 1, dataE);
    leerNombresDeArchivos(dataE, nFiles, fileNames, fileChars);

    PriorityQueue* pQueue = leerArbolHuffman(dataE);
    Node* arbol_huffman = construir_arbol_huffman(pQueue);

    createDirectory("tmp"); 
    createDirectory("decoded"); 
    separateChunks(dataE, fileNames, nFiles);

    printf("nFiles: %d\n", nFiles);

    // Cierra el archivo de entrada
    fclose(dataE);

    DIR *dir;
    struct dirent *dp;

    if ((dir = opendir("tmp")) == NULL) {
        perror("Cannot open directory");
        return 1;
    }

    int i = 0;
    while ((dp = readdir(dir))) {
        if (dp->d_type == DT_REG) {
            pid_t pid = fork();

            if (pid == 0) {
                // Proceso hijo: abrir y leer el archivo chunk
                char chunkFilePath[256];
                snprintf(chunkFilePath, sizeof(chunkFilePath), "tmp/%s", dp->d_name);
                
                FILE *chunkFile = fopen(chunkFilePath, "rb");
                if (chunkFile == NULL) {
                    perror("Error opening chunk file");
                    exit(1);
                }
                rebuidFile(dp->d_name, chunkFile, fileChars[i], arbol_huffman);
                fclose(chunkFile);
                exit(0);  // Terminar el proceso hijo después de completar su tarea

            } else if (pid > 0) {
                // Proceso padre: continuar con el siguiente archivo
            } else {
                perror("Error en fork()");
                exit(1);
            }
            i++;
        }
    }

    // Cerrar el directorio
    closedir(dir);

    while (wait(NULL) > 0);

    eliminarDirectorio("tmp");
    if (gettimeofday(&end, NULL) != 0) {
        perror("Error getting end time");
        exit(EXIT_FAILURE);
    }

    // Calcular el tiempo transcurrido
    elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    printf("Tiempo transcurrido: %f segundos\n", elapsed_time);

    return 0;
}