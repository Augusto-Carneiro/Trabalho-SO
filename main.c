#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

int tam_matriz;
int num_threads;

void inicializar_variavel_global(char *nome_variavel, int valor);
int* alocar_matriz();
int ler_elementos_matriz(FILE* doc, int *matriz, const char *arquivo);
void *ler_matriz_threads(void *nome_arquivo);
void Salvar_Matriz(char *nome_arquivo, int *matriz);
void somar_matrizes(int *A, int *B, int *D);
void multiplicar_matrizes(int *C, int *D, int *E);
int reduzir_matriz(int *E);

int main(int argc, char *argv[]){
   
    int T = atoi(argv[1]); // Número de threads de processamento
    int n = atoi(argv[2]); // Número de linhas e colunas das matrizes
    const char *arqA = argv[3];
    const char *arqB = argv[4];
    const char *arqC = argv[5];
    const char *arqD = argv[6];
    const char *arqE = argv[7];

    inicializar_variavel_global("tam_matriz", T);
    inicializar_variavel_global("num_threads", n);

    printf("%d %d \n", n, T);


    return 0;
}

void inicializar_variavel_global(char *nome_variavel, int valor){
    if(strcmp(nome_variavel, "tam_matriz") == 0){
        tam_matriz = valor;
    }else if(strcmp(nome_variavel, "num_threads") == 0){
        num_threads = valor;
    }else{
        printf("Erro: Variável global %s não encontrada.\n", nome_variavel);
    }
}

int* alocar_matriz(){
    int* matriz = (int *)malloc(pow(tam_matriz, 2) * sizeof(int));
    if(matriz == NULL){
        printf("Erro ao alocar memória para a matriz\n");
        return NULL;
    }
    return matriz;
}

void* ler_matriz_threads(void *nome_arquivo){
    const char *arquivo_lido = nome_arquivo;

    FILE* doc = fopen(arquivo_lido, "r"); //Abrir o arquivo
    if (doc == NULL) {
        printf("Erro ao abrir o arquivo %s\n", arquivo_lido);
        pthread_exit(NULL); 
    }

    int* matriz = alocar_matriz(); // Alocar espaço para a matriz usando a função alocar_matriz

    if(matriz == NULL){
        printf("Erro ao alocar memória para a matriz ao ler do arquivo %s\n", arquivo_lido);
        fclose(doc);
        pthread_exit(NULL);
    }

    if(!ler_elementos_matriz(doc, matriz, arquivo_lido)){ // Lendo os elementos da matriz usando a função separada
        free(matriz);
        fclose(doc);
        pthread_exit(NULL);
    }

    fclose(doc);

    pthread_exit((void *)matriz); // Retornando a matriz lida para a thread principal.
}

int ler_elementos_matriz(FILE* doc, int *matriz, const char *arquivo){
    for(int i = 0; i < pow(tam_matriz, 2); i++){
        if(fscanf(doc, "%d", &matriz[i]) != 1){
            printf("Erro ao ler dados do arquivo %s\n", arquivo);
            return 0; // Teve erro na leitura.
        }
    }
    return 1; // Leu certo.
}

void Salvar_Matriz(char *nome_arquivo, int *matriz){
    FILE* arquivo_lido = fopen(nome_arquivo, "w"); // Abro o arquivo que quero salvar a matriz.
    if(arquivo_lido == NULL){
        printf("Erro ao abrir o arquivo %s", nome_arquivo);
        return;
    }

    for(int i = 0; i < tam_matriz; i++){ // Escrever a matriz no arquivo.
        for(int j = 0; j < tam_matriz; j++){
            fprintf(arquivo_lido, "%d ", *(matriz + i * tam_matriz + j));
        }
        fprintf(arquivo_lido, "\n");
    }

    fclose(arquivo_lido);
}

void somar_matrizes(int *A, int *B, int *D){
    for(int i = 0; i < pow(tam_matriz, 2); i++){
        D[i] = A[i] + B[i];
    }
}

void multiplicar_matrizes(int *C, int *D, int *E){
    int soma;
    for(int i = 0; i < tam_matriz; i++){
        for(int j = 0; j < tam_matriz; j++){
            soma = 0;
            for (int coluna = 0; coluna < tam_matriz; coluna++){
                soma += C[i * tam_matriz + coluna] * D[coluna * tam_matriz + j];
            }
            E[i * tam_matriz + j] = soma;
        }
    }
}

int reduzir_matriz(int *E){
    int soma = 0;
    for(int i = 0; i < pow(tam_matriz, 2); i++){
        soma += E[i];
    }
    return soma;
}