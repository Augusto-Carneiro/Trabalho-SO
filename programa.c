#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<string.h>
#include<math.h>
#include<sys/time.h>

int tam_matriz;  //Variáveis globais para ajudar nos parâmetros.
int num_threads;
int reducao = 0;
int *A, *B, *C, *D, *E;

void inicializar_variavel_global(char* nome_variavel, int valor); //Protótipos das funções.
int* alocar_matriz();
void* ler_matriz_threads(void* nome_arquivo);
int ler_elementos_matriz(FILE* doc, int *matriz, const char *arquivo);
void Salvar_Matriz(const char* nome_arquivo, int* matriz);
void* salvar_matrizD_thread(void* nome_arquivo);
void* salvar_matrizE_thread(void* nome_arquivo);
void* somar_matriz(void* arg);
void* multiplicar_matriz(void* arg);
void* Reducao_matriz(void* arg);
double calcular_tempo(struct timeval inicio, struct timeval fim);

int main(int argc, char* argv[]){

    int T = atoi(argv[1]); // Número de threads de processamento.
    int n = atoi(argv[2]); // Número de linhas e colunas das matrizes.
    const char *arqA = argv[3]; //Leitura do nome dos arquivos.
    const char *arqB = argv[4];
    const char *arqC = argv[5];
    const char *arqD = argv[6];
    const char *arqE = argv[7];

    inicializar_variavel_global("tam_matriz", n); //Criei 3 váriaveis globais para facilitar o uso de certas informações.
    inicializar_variavel_global("num_threads", T);

    D = alocar_matriz(); //Alocaçao da mémoria para as matrizes que serão geradas
    E = alocar_matriz();

    pthread_t threadA, threadB, threadC, threadD, threadE, threadReducao, threads[num_threads]; //Threads criadas para ajudar na distinção de cada etapa.
    int i, controle_thread[num_threads];

    struct timeval inicio_total, fim_total;
    gettimeofday(&inicio_total, NULL);

    //Passo 1
    pthread_create(&threadA, NULL, ler_matriz_threads, (void *)arqA); //Leitura da matriz A por thread
    pthread_create(&threadB, NULL, ler_matriz_threads, (void *)arqB); //Leitura da matriz B por thread

    pthread_join(threadA, (void** )&A);
    pthread_join(threadB, (void** )&B);  //Esperando as 2 finalizarem 

    //imprimir_matriz("Matriz A", A);
    //imprimir_matriz("Matriz B", B);     

    struct timeval inicio_soma, fim_soma;

    gettimeofday(&inicio_soma, NULL); //inicia a contagem do tempo para soma.

    //Passo 2
    for(i = 0; i < num_threads; i++){ //Faz a soma criando um array para as threads.
        controle_thread[i] = i;
        pthread_create(&threads[i], NULL, somar_matriz, &controle_thread[i]);
    }

    for(i = 0; i < num_threads; i++){ // Esperar que todas as threads terminem.
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&fim_soma, NULL); // Acaba o tempo da soma.
    double tempo_soma = calcular_tempo(inicio_soma, fim_soma); //Calcula o tempo da soma.

    //imprimir_matriz("Matriz D:", D);

    //Passo 3 e 4
    pthread_create(&threadC, NULL, ler_matriz_threads, (void* )arqC); // 1 thread para cada etapa.
    pthread_create(&threadD, NULL, salvar_matrizD_thread, (void* )arqD);

    pthread_join(threadC, (void** )&C); // Esperar a leitura da matriz C e salvar a matriz D.
    pthread_join(threadD, NULL);

    //imprimir_matriz("Matriz C:", C);

    struct timeval inicio_multiplicacao, fim_multiplicacao;
    gettimeofday(&inicio_multiplicacao, NULL); //inicia o tempo de multiplicação.

    //Passo 5
    for(i = 0; i < num_threads; i++){ //Faz a multiplicação criando um array para as threads.
        controle_thread[i] = i;
        pthread_create(&threads[i], NULL, multiplicar_matriz, &controle_thread[i]);
    }
 
    for(i = 0; i < num_threads; i++){ // Esperar que todas as threads terminem
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&fim_multiplicacao, NULL); //Finaliza o tempo da multiplicação.
    double tempo_multiplicacao = calcular_tempo(inicio_multiplicacao, fim_multiplicacao); //Calcula o tempo da multiplicação.

    struct timeval inicio_reducao, fim_reducao;
    gettimeofday(&inicio_reducao, NULL); //inicia o tempo da redução.

    //Passo 6 e 7
    pthread_create(&threadE, NULL, salvar_matrizE_thread, (void *)arqE); //Usa 1 thread para salvar a matriz E e T threads(na função) na redução.
    pthread_create(&threadReducao, NULL, Reducao_matriz, NULL);

    pthread_join(threadE, NULL); //Espera ambos acabarem
    pthread_join(threadReducao, NULL);

    gettimeofday(&fim_reducao, NULL); //Finaliza o tempo da redução.
    double tempo_reducao = calcular_tempo(inicio_reducao, fim_reducao);

    free(A);  //Liberar a memória alocada.
    free(B);
    free(C);
    free(D);
    free(E);

    gettimeofday(&fim_total, NULL); //Pega o tempo do fim do programa.
    double tempo_total = calcular_tempo(inicio_total, fim_total); //Calculo do tempo total.

    printf("Redução: %d\n", reducao); //Prints conforme pedido.
    printf("Tempo soma: %.3lf segundos.\n", tempo_soma);
    printf("Tempo multiplicação: %.3lf segundos.\n", tempo_multiplicacao);
    printf("Tempo redução: %.3lf segundos.\n", tempo_reducao);
    printf("Tempo total: %.3lf segundos.\n", tempo_total);

    return 0;
}

void inicializar_variavel_global(char *nome_variavel, int valor){ //Função que da valor para as váriaveis globais.
    if(strcmp(nome_variavel, "tam_matriz") == 0){
        tam_matriz = valor;
    }else if(strcmp(nome_variavel, "num_threads") == 0){
        num_threads = valor;
    }else if(strcmp(nome_variavel, "reducao") == 0){
        reducao = valor;    
    }else{
        printf("Erro: Variável global %s não encontrada.\n", nome_variavel);
    }
}

int* alocar_matriz(){ //Função para alocar o espaço para as matrizes nxn.
    int* matriz = (int *)malloc(pow(tam_matriz, 2) * sizeof(int));
    if(matriz == NULL){
        printf("Erro ao alocar memória para a matriz\n");
        return NULL;
    }
    return matriz;
}

void* ler_matriz_threads(void *nome_arquivo){
    const char* arquivo_lido = nome_arquivo;

    FILE* doc = fopen(arquivo_lido, "r"); //Abrir o arquivo.
    if(doc == NULL){
        printf("Erro ao abrir o arquivo %s\n", arquivo_lido);
        pthread_exit(NULL); 
    }

    int* matriz = alocar_matriz(); // Alocar espaço para a matriz usando a função alocar_matriz.

    if(matriz == NULL){ //Caso falhe.
        printf("Erro ao alocar memória para a matriz ao ler do arquivo %s\n", arquivo_lido);
        fclose(doc);
        pthread_exit(NULL);
    }

    if(!ler_elementos_matriz(doc, matriz, arquivo_lido)){ // Lendo os elementos da matriz usando a função separada.
        free(matriz);
        fclose(doc);
        pthread_exit(NULL);
    }

    fclose(doc);

    pthread_exit((void *)matriz); // Retornando a matriz lida para a thread principal.
}

int ler_elementos_matriz(FILE* doc, int *matriz, const char *arquivo){
    for(int i = 0; i < pow(tam_matriz, 2); i++){
        if(fscanf(doc, "%d", &matriz[i]) != 1){ //Lê o arquivo e pega a matriz.
            printf("Erro ao ler dados do arquivo %s\n", arquivo);
            return 0; // Teve erro na leitura.
        }
    }
    return 1; // Leu certo.
}

void Salvar_Matriz(const char *nome_arquivo, int *matriz){
    int i, j;
    FILE* arquivo_lido = fopen(nome_arquivo, "w"); // Abro o arquivo que quero salvar a matriz.
    if(arquivo_lido == NULL){
        printf("Erro ao abrir o arquivo %s", nome_arquivo);
        return;
    }

    for(i = 0; i < tam_matriz; i++){ // Escrever a matriz no arquivo.
        for(j = 0; j < tam_matriz; j++){
            fprintf(arquivo_lido, "%d ", *(matriz + i * tam_matriz + j));
        }
        fprintf(arquivo_lido, "\n");
    }

    fclose(arquivo_lido);
}

void* somar_matriz(void* arg){
    int controle_thread = *((int*)arg); //Converte o argumento recebido (um ponteiro void*) para um inteiro que representa o índice da thread criado na main.
    int linhas_por_thread = tam_matriz / num_threads; //Vê quantas linhas cada thread vai processar dividindo o tamanho da matriz pelo número de threads.
    int inicio = controle_thread * linhas_por_thread; //Determina o índice inicial.
    int fim = inicio + linhas_por_thread - 1; //Determina o índice final.

    if(controle_thread == num_threads - 1){  // A última thread pode precisar lidar com as linhas q sobraram.
        fim = tam_matriz - 1;  // Última linha da matriz.
    }

    for(int i = inicio; i <= fim; i++){
        for(int j = 0; j < tam_matriz; j++){
            D[i * tam_matriz + j] = A[i * tam_matriz + j] + B[i * tam_matriz + j]; // Faz a soma dos termos.
        }
    }

    pthread_exit(NULL);
}

void imprimir_matriz(const char* nome, int* matriz){ //Função auxiliar para checar se as leituras estavam dando certo.
    int i, j;
    printf("%s:\n", nome);
    for(i = 0; i < tam_matriz; i++){
        for(j = 0; j < tam_matriz; j++){
            printf("%d ", matriz[i * tam_matriz + j]);
        }
        printf("\n");
    }
    printf("\n");
}

void* salvar_matrizD_thread(void *nome_arquivo){ //Fiz uma função separada para salvar a matriz D.
    const char *arquivo = nome_arquivo;
    Salvar_Matriz(arquivo, D);
    pthread_exit(NULL);
}

void* salvar_matrizE_thread(void *nome_arquivo){ //Fiz uma função separada para salvar a matriz E.
    const char *arquivo = nome_arquivo;
    Salvar_Matriz(arquivo, E);
    pthread_exit(NULL);
}

void* multiplicar_matriz(void* arg){ //Semelhante a somar_matriz.
    int i, j;
    int controle_thread = *((int*)arg);
    int linhas_por_thread = tam_matriz / num_threads;
    int inicio = controle_thread * linhas_por_thread;
    int fim = inicio + linhas_por_thread - 1;

    if(controle_thread == num_threads - 1){
        fim = tam_matriz - 1;  
    }

    for(i = inicio; i <= fim; i++){ //Faz a operação de multiplicação. 
        for(j = 0; j < tam_matriz; j++){
            E[i * tam_matriz + j] = 0;
            for(int k = 0; k < tam_matriz; k++){
                E[i * tam_matriz + j] += A[i * tam_matriz + k] * B[k * tam_matriz + j];
            }
        }
    }

    pthread_exit(NULL);
}

void* Reducao_matriz(void* arg){ //Semelhante as outras (somar e multiplicar).
    int i, soma_total = 0;

    int linhas_por_thread = tam_matriz / num_threads;
    int inicio, fim;

    for(i = 0; i < num_threads; i++){ // Dividir o trabalho entre as threads
        inicio = i * linhas_por_thread;
        fim = inicio + linhas_por_thread - 1;

        if(i == num_threads - 1){
            fim = tam_matriz - 1;
        }

        for(int linha = inicio; linha <= fim; linha++){ // Somar os elementos da matriz E para essa parte
            for(int coluna = 0; coluna < tam_matriz; coluna++){
                soma_total += E[linha * tam_matriz + coluna];
            }
        }

    inicializar_variavel_global("reducao", soma_total); //Por ser void, pensei em usar essa função para atualizar o

    }

    pthread_exit(NULL);
}

double calcular_tempo(struct timeval inicio, struct timeval fim){ //Mudei a forma de pegar os horários pq estava bugando o tempo, a função pegava quase o dobro do tempo passado.
    return (double)(fim.tv_sec - inicio.tv_sec) + (double)(fim.tv_usec - inicio.tv_usec) / 1000000.0;
}
