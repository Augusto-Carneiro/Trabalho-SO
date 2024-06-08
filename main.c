#include<stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    if(argc != 8){
        printf("A linha de comando foi digitada errada!\n"); //Verificação se o comando no terminal está correto.
        return 1;
    }

    int T = atoi(argv[1]); // Peguei o número de threads 
    int n = atoi(argv[2]); // Peguei o número de linhas e colunas 

    // Pra checar:
    //printf("Numero de threads: %d\n", T);
    //printf("Linhas e colunas das matrizes: %d\n", n);

    return 0;
}

