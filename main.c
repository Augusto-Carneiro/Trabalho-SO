#include<stdio.h>
#include <stdlib.h>
#include<math.h>

int* ler_matriz(const char* nome_arquivo, int n);
void Salvar_Matriz(char *nome_arquivo, int *matriz, int n);

int main(int argc, char* argv[]){
    int T = atoi(argv[1]); // Peguei o número de threads 
    int n = atoi(argv[2]); // Peguei o número de linhas e colunas 

    // Pra checar:
    //printf("Numero de threads: %d\n", T);
    //printf("Linhas e colunas das matrizes: %d\n", n);

    return 0;
}

int* ler_matriz(const char* nome_arquivo, int n){
    FILE *arquivo;
    int *matriz;
    int i, j;

    arquivo = fopen(nome_arquivo, "r"); // Abre o arquivo para leitura
    if(arquivo == NULL){
        printf("Erro ao abrir o arquivo %s\n", nome_arquivo);
        return NULL;
    }

    matriz = (int*) malloc(pow(n,2) * sizeof(int)); // Aloca memória para a matriz em uma única chamada, conforme pedido.
    if (matriz == NULL){
        printf("Erro ao alocar memória para a matriz do arquivo %s\n", nome_arquivo);
        fclose(arquivo);
        return NULL;
    }

    for (i = 0; i < n; i++){
        for (j = 0; j < n; j++){
            if (fscanf(arquivo, "%d", &matriz[i * n + j]) != 1){ // Lê os valores da matriz do arquivo
                printf("Erro ao ler dados do arquivo %s\n", nome_arquivo);
                free(matriz);
                fclose(arquivo);
                return NULL;
            }
        }
    }
    fclose(arquivo);

    return matriz;
}

void Salvar_Matriz(char *nome_arquivo, int *matriz, int n){
    FILE *arquivo = fopen(nome_arquivo, "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s", nome_arquivo);
        return 1;
    }

    for (int i = 0; i < n; i++){ // Escrever a matriz no arquivo
        for (int j = 0; j < n; j++){
            fprintf(arquivo, "%d ", *(matriz + i * n + j));
        }
        fprintf(arquivo, "\n");
    }

    fclose(arquivo);
}