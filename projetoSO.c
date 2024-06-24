#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

//Estrutura para representar uma matriz
typedef struct {
    int *dados;
    int n;
} Matriz;

//Estrutura para passar dados para as threads
typedef struct {
    Matriz *A;
    Matriz *B;
    Matriz *resultado;
    int linhaComeco;
    int linhaFinal;
    int n;
} DadosThread;

//Funcao para alocar memoria para uma matriz n x n 
Matriz alocacaoMatriz(int n) {
    Matriz matriz;
    matriz.n = n;
    matriz.dados = (int *) malloc(n * n * sizeof(int));
    return matriz;
}

//Funcao para liberar memoria de uma matriz
void freeMatriz(Matriz matriz) {
    free(matriz.dados);
}

//Funcao para ler os dados de uma matriz de um arquivo
void leituraMatriz(Matriz *matriz, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r"); //abre o arquivo para leitura
    if (!arquivo) {
        perror("Erro ao abrir arquivo");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < matriz->n; i++) {
        for (int j = 0; j < matriz->n; j++) {
            //le os dados do arquivo
            if (fscanf(arquivo, "%d", &matriz->dados[i * matriz->n + j]) != 1) {
                fprintf(stderr, "Erro ao ler o valor da matriz no arquivo %s\n", nomeArquivo);
                fclose(arquivo);
                exit(EXIT_FAILURE);
            }
        }
    }
    fclose(arquivo); //fecha o arquivo
}

//funcao para gravar os dados de uma matriz em um arquivo
void gravarMatriz(Matriz *matriz, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "w"); //abre o arquivo para escrita
    if (!arquivo) {
        perror("Erro ao abrir arquivo");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < matriz->n; i++) {
        for (int j = 0; j < matriz->n; j++) {
            fprintf(arquivo, "%d ", matriz->dados[i * matriz->n + j]); //escreve os dados no arquivo
        }
        fprintf(arquivo, "\n");
    }
    fclose(arquivo); //fecha o arquivo
}

//funcao para limpar o conteúdo de um arquivo
void limparArquivo(const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "w");
    if (!arquivo) {
        perror("Erro ao abrir arquivo para limpeza");
        exit(EXIT_FAILURE);
    }
    fclose(arquivo);
}

//funcao executada pelas threads para somar matrizes
void* somaMatrizes_thread(void *arg) {
    DadosThread *dados = (DadosThread *) arg; // converte o argumento para a estrutura DadosThread
    for (int i = dados->linhaComeco; i < dados->linhaFinal; i++) {
        for (int j = 0; j < dados->n; j++) {
            dados->resultado->dados[i * dados->n + j] = dados->A->dados[i * dados->n + j] + dados->B->dados[i * dados->n + j];
        }
    }
    return NULL;
}

//funcao para criar threads e realizar a soma de matrizes
void somaMatrizes(Matriz *A, Matriz *B, Matriz *D, int numeroThreads) {
    pthread_t threads[numeroThreads];
    DadosThread dadosThread[numeroThreads];
    int linhasPorThread = A->n / numeroThreads;

    for (int i = 0; i < numeroThreads; i++) {
        dadosThread[i].A = A;
        dadosThread[i].B = B;
        dadosThread[i].resultado = D;
        dadosThread[i].linhaComeco = i * linhasPorThread;
        dadosThread[i].linhaFinal = (i == numeroThreads - 1) ? A->n : (i + 1) * linhasPorThread;
        dadosThread[i].n = A->n;
        pthread_create(&threads[i], NULL, somaMatrizes_thread, &dadosThread[i]); // cria a thread
    }

    
    for (int i = 0; i < numeroThreads; i++) { //aguarda o término das threads
        pthread_join(threads[i], NULL);
    }
}

//funcao executada pelas threads para multiplicar matrizes
void* multiplicacaoMatrizes_thread(void *arg) {
    DadosThread *dados = (DadosThread *) arg; //converte o argumento para a estrutura DadosThread
    for (int i = dados->linhaComeco; i < dados->linhaFinal; i++) {
        for (int j = 0; j < dados->n; j++) {
            dados->resultado->dados[i * dados->n + j] = 0; // inicializa o valor da posição da matriz resultado
            for (int k = 0; k < dados->n; k++) {
                dados->resultado->dados[i * dados->n + j] += dados->A->dados[i * dados->n + k] * dados->B->dados[k * dados->n + j];
            }
        }
    }
    return NULL;
}

// funcao para criar threads e realizar a multiplicação de matrizes
void multiplicacaoMatrizes(Matriz *C, Matriz *D, Matriz *E, int numeroThreads) {
    pthread_t threads[numeroThreads];
    DadosThread dadosThread[numeroThreads];
    int linhasPorThread = C->n / numeroThreads;

    for (int i = 0; i < numeroThreads; i++) {
        dadosThread[i].A = C;
        dadosThread[i].B = D;
        dadosThread[i].resultado = E;
        dadosThread[i].linhaComeco = i * linhasPorThread;
        dadosThread[i].linhaFinal = (i == numeroThreads - 1) ? C->n : (i + 1) * linhasPorThread;
        dadosThread[i].n = C->n;
        pthread_create(&threads[i], NULL, multiplicacaoMatrizes_thread, &dadosThread[i]);
    }

    for (int i = 0; i < numeroThreads; i++) {
        pthread_join(threads[i], NULL);
    }
}

// funcao para realizar a redução de uma matriz (somar todos os elementos)
int reducaoMatriz(Matriz *E) {
    int soma = 0;
    for (int i = 0; i < E->n; i++) {
        for (int j = 0; j < E->n; j++) {
            soma += E->dados[i * E->n + j];
        }
    }
    return soma;
}

// funcao para calcular o tempo atual em segundos
double calcularTempo() {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

int main(int argc, char *argv[]) {

    // verifica se o numero de argumentos eh correto
    if (argc != 8) {
        fprintf(stderr, "Uso: %s T n arqA.dat arqB.dat arqC.dat arqD.dat arqE.dat\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // le os argumentos da linha de comando
    int T = atoi(argv[1]);
    int n = atoi(argv[2]);
    const char *arqA = argv[3];
    const char *arqB = argv[4];
    const char *arqC = argv[5];
    const char *arqD = argv[6];
    const char *arqE = argv[7];

    // aloca a memoria para as matrizes
    Matriz A = alocacaoMatriz(n);
    Matriz B = alocacaoMatriz(n);
    Matriz C = alocacaoMatriz(n);
    Matriz D = alocacaoMatriz(n);
    Matriz E = alocacaoMatriz(n);

    // le os dados das matrizes dos arquivos
    leituraMatriz(&A, arqA);
    leituraMatriz(&B, arqB);
    leituraMatriz(&C, arqC);

    // limpa os arquivos de saída
    limparArquivo(arqD); 
    limparArquivo(arqE); 

    double inicio, fim;
    double tempoInicio, tempoFinal;

    // marca o tempo de início total
    tempoInicio = calcularTempo();

    // realiza a soma das matrizes A e B, armazenando o resultado em D
    inicio = calcularTempo();
    somaMatrizes(&A, &B, &D, T);
    fim = calcularTempo();
    double tempoSoma = fim - inicio;
    gravarMatriz(&D, arqD);  // grava a matriz resultado da soma no arquivo

    // realiza a multiplicação das matrizes C e D, armazenando o resultado em E
    inicio = calcularTempo();
    multiplicacaoMatrizes(&C, &D, &E, T);
    fim = calcularTempo();
    double tempoMultiplicacao = fim - inicio;
    gravarMatriz(&E, arqE); // grava a matriz resultado da multiplicação no arquivo

    // realiza a redução da matriz E (soma de todos os elementos)
    inicio = calcularTempo();
    int resultado_reducao = reducaoMatriz(&E);
    fim = calcularTempo();
    double tempoReducao = fim - inicio;

    // marca o tempo de fim total
    tempoFinal = calcularTempo();
    double tempoTotal = tempoFinal - tempoInicio;

    // imprime os resultados e os tempos de execução
    printf("Redução: %d\n", resultado_reducao);
    printf("Tempo soma: %.6f segundos.\n", tempoSoma);
    printf("Tempo multiplicação: %.6f segundos.\n", tempoMultiplicacao);
    printf("Tempo redução: %.6f segundos.\n", tempoReducao);
    printf("Tempo total: %.6f segundos.\n", tempoTotal);

    // libera a memória alocada para as matrizes
    freeMatriz(A);
    freeMatriz(B);
    freeMatriz(C);
    freeMatriz(D);
    freeMatriz(E);

    return 0;
}
