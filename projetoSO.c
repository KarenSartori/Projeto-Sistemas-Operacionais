#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct {
    int *dados;
    int n;
} Matriz;

typedef struct {
    Matriz *A;
    Matriz *B;
    Matriz *resultado;
    int linhaComeco;
    int linhaFinal;
    int n;
} DadosThread;

Matriz alocacaoMatriz(int n) {
    Matriz matriz;
    matriz.n = n;
    matriz.dados = (int *) malloc(n * n * sizeof(int));
    return matriz;
}

void freeMatriz(Matriz matriz) {
    free(matriz.dados);
}

void leituraMatriz(Matriz *matriz, const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) {
        perror("Erro ao abrir arquivo");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < matriz->n; i++) {
        for (int j = 0; j < matriz->n; j++) {
            if (fscanf(arquivo, "%d", &matriz->dados[i * matriz->n + j]) != 1) {
                fprintf(stderr, "Erro ao ler o valor da matriz no arquivo %s\n", nome_arquivo);
                fclose(arquivo);
                exit(EXIT_FAILURE);
            }
        }
    }
    fclose(arquivo);
}

void gravarMatriz(Matriz *matriz, const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "w");
    if (!arquivo) {
        perror("Erro ao abrir arquivo");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < matriz->n; i++) {
        for (int j = 0; j < matriz->n; j++) {
            fprintf(arquivo, "%d ", matriz->dados[i * matriz->n + j]);
        }
        fprintf(arquivo, "\n");
    }
    fclose(arquivo);
}

void limparArq(const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "w");
    if (!arquivo) {
        perror("Erro ao abrir arquivo para limpeza");
        exit(EXIT_FAILURE);
    }
    fclose(arquivo);
}

void* somaMatrizes_thread(void *arg) {
    DadosThread *dados = (DadosThread *) arg;
    for (int i = dados->linhaComeco; i < dados->linhaFinal; i++) {
        for (int j = 0; j < dados->n; j++) {
            dados->resultado->dados[i * dados->n + j] = dados->A->dados[i * dados->n + j] + dados->B->dados[i * dados->n + j];
        }
    }
    return NULL;
}

void somaMatrizes(Matriz *A, Matriz *B, Matriz *D, int num_threads) {
    pthread_t threads[num_threads];
    DadosThread dados_thread[num_threads];
    int linhas_por_thread = A->n / num_threads;

    for (int i = 0; i < num_threads; i++) {
        dados_thread[i].A = A;
        dados_thread[i].B = B;
        dados_thread[i].resultado = D;
        dados_thread[i].linhaComeco = i * linhas_por_thread;
        dados_thread[i].linhaFinal = (i == num_threads - 1) ? A->n : (i + 1) * linhas_por_thread;
        dados_thread[i].n = A->n;
        pthread_create(&threads[i], NULL, somaMatrizes_thread, &dados_thread[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
}

void* multiplicacaoMatrizes_thread(void *arg) {
    DadosThread *dados = (DadosThread *) arg;
    for (int i = dados->linhaComeco; i < dados->linhaFinal; i++) {
        for (int j = 0; j < dados->n; j++) {
            dados->resultado->dados[i * dados->n + j] = 0;
            for (int k = 0; k < dados->n; k++) {
                dados->resultado->dados[i * dados->n + j] += dados->A->dados[i * dados->n + k] * dados->B->dados[k * dados->n + j];
            }
        }
    }
    return NULL;
}

void multiplicacaoMatrizes(Matriz *C, Matriz *D, Matriz *E, int num_threads) {
    pthread_t threads[num_threads];
    DadosThread dados_thread[num_threads];
    int linhas_por_thread = C->n / num_threads;

    for (int i = 0; i < num_threads; i++) {
        dados_thread[i].A = C;
        dados_thread[i].B = D;
        dados_thread[i].resultado = E;
        dados_thread[i].linhaComeco = i * linhas_por_thread;
        dados_thread[i].linhaFinal = (i == num_threads - 1) ? C->n : (i + 1) * linhas_por_thread;
        dados_thread[i].n = C->n;
        pthread_create(&threads[i], NULL, multiplicacaoMatrizes_thread, &dados_thread[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
}

int reducaoMatriz(Matriz *E) {
    int soma = 0;
    for (int i = 0; i < E->n; i++) {
        for (int j = 0; j < E->n; j++) {
            soma += E->dados[i * E->n + j];
        }
    }
    return soma;
}

double calcularTempo() {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

int main(int argc, char *argv[]) {
    if (argc != 8) {
        fprintf(stderr, "Uso: %s T n arqA.dat arqB.dat arqC.dat arqD.dat arqE.dat\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int T = atoi(argv[1]);
    int n = atoi(argv[2]);
    const char *arqA = argv[3];
    const char *arqB = argv[4];
    const char *arqC = argv[5];
    const char *arqD = argv[6];
    const char *arqE = argv[7];

    Matriz A = alocacaoMatriz(n);
    Matriz B = alocacaoMatriz(n);
    Matriz C = alocacaoMatriz(n);
    Matriz D = alocacaoMatriz(n);
    Matriz E = alocacaoMatriz(n);

    leituraMatriz(&A, arqA);
    leituraMatriz(&B, arqB);
    leituraMatriz(&C, arqC);

    limparArq(arqD); 
    limparArq(arqE); 

    double inicio, fim;
    double inicio_total, fim_total;

    inicio_total = calcularTempo();

    inicio = calcularTempo();
    somaMatrizes(&A, &B, &D, T);
    fim = calcularTempo();
    double tempo_soma = fim - inicio;
    gravarMatriz(&D, arqD);

    inicio = calcularTempo();
    multiplicacaoMatrizes(&C, &D, &E, T);
    fim = calcularTempo();
    double tempo_multiplicacao = fim - inicio;
    gravarMatriz(&E, arqE);

    inicio = calcularTempo();
    int resultado_reducao = reducaoMatriz(&E);
    fim = calcularTempo();
    double tempo_reducao = fim - inicio;

    fim_total = calcularTempo();
    double tempo_total = fim_total - inicio_total;

    printf("Redução: %d\n", resultado_reducao);
    printf("Tempo soma: %.6f segundos.\n", tempo_soma);
    printf("Tempo multiplicação: %.6f segundos.\n", tempo_multiplicacao);
    printf("Tempo redução: %.6f segundos.\n", tempo_reducao);
    printf("Tempo total: %.6f segundos.\n", tempo_total);

    freeMatriz(A);
    freeMatriz(B);
    freeMatriz(C);
    freeMatriz(D);
    freeMatriz(E);

    return 0;
}
