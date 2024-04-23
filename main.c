#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "timer.h"

/* Valor global do programa, em que as threads irão verificar se são primos ou não
 * e a cada passagem dos locks, incrementar o próximo para a próxima thread executar
 */
long long int numero = 0;

/* Resultado global da contagem de primos, onde cada thread irá somar 1 se o valor
 * que ela analisou for primo, ou 0 se não for, incrementado a cada retorno das threads.
 */
long long int qtdPrimos = 0;

long long int N;

pthread_mutex_t mutex; //variavel de lock para exclusao mutua

// estrutura auxiliar para montar resultados para um csv:
typedef struct {
    long long int N;
    int qtdThreads;
    double aceleracao;
    double eficiencia;
    double tempoExecucao;
    double tempoSequencial;
} Experimento;

//método para extrair resultados para análise em um csv, dada uma lista de experimentos
void extrairCsv(Experimento * experimento, char * nomeArquivo) {

    FILE * resultados;
    resultados = fopen(nomeArquivo, "r");

    if (!resultados) {
        resultados = fopen(nomeArquivo, "a");
        fprintf(resultados, "qtdNumeros;qtdThreads;aceleracao;eficiencia;tempoExecucao;tempoSequencial\n");
        //escreve os experimento resultantes do experimento em uma linha:
        fprintf(resultados,"%lld;%d;%f;%f;%f;%f\n", experimento->N, experimento->qtdThreads, experimento->aceleracao, experimento->eficiencia,
                experimento->tempoExecucao, experimento->tempoSequencial);
    } else {
        resultados = fopen(nomeArquivo, "a");
        //escreve os experimento resultantes do experimento em uma linha:
        fprintf(resultados,"%lld;%d;%f;%f;%f;%f\n", experimento->N, experimento->qtdThreads, experimento->aceleracao, experimento->eficiencia,
                experimento->tempoExecucao, experimento->tempoSequencial);
    }

    if (!resultados) {
        fprintf(stderr, "Erro ao criar arquivo csv\n");
        return;
    }

    fclose(resultados);
}

int ehPrimo(long long int n) {
    int i;
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (i = 3; i < sqrt(n) + 1; i += 2)
        if (n % i == 0) return 0;
    return 1;
}

void * tarefa(void * arg) {

    while (numero != N && numero < N) {

        pthread_mutex_lock(&mutex);
        long long int n = numero;
        numero++;
        pthread_mutex_unlock(&mutex);

        int primo = ehPrimo(n);

        pthread_mutex_lock(&mutex);
        qtdPrimos += primo;
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

void criarThreads(int M) {

    // recuperando o id das threads no sistema:
    pthread_t tid[M];
    int t, id[M];

    //--inicializa o mutex (lock de exclusao mutua)
    pthread_mutex_init(&mutex, NULL);

    for(int t=0; t<M; t++) {
        id[t]=t;
        if (pthread_create(&tid[t], NULL, tarefa, (void *) &id[t])) {
            printf("--ERRO: pthread_create()\n"); exit(-1);
        }
    }
    //--espera todas as threads terminarem
    for (t=0; t<M; t++) {
        if (pthread_join(tid[t], NULL)) {
            printf("--ERRO: pthread_join() \n"); exit(-1);
        }
    }

    pthread_mutex_destroy(&mutex);
}


int geraResultados(int M, long long int N) {

    double inicio, fim;

    GET_TIME(inicio);
    criarThreads(1);
    GET_TIME(fim);
    double tempoSequencial = fim - inicio;

    double tempoMedioConcorrente = 0;
    int numNucleos = 4;

    for (int i = 0; i<5; i++) {
        numero = 0;
        qtdPrimos = 0;
        GET_TIME(inicio);
        criarThreads(M);
        GET_TIME(fim);
        //extrai tempo médio de processamento
        tempoMedioConcorrente += (fim - inicio)/5;
    }
    printf("Tempo decorrido (Concorrente): %f segundos\nContagem de primalidade em %lld números com %d threads\n", tempoMedioConcorrente, N, M);

    Experimento * exp = malloc(sizeof (Experimento));
    exp->tempoExecucao = tempoMedioConcorrente;
    exp->aceleracao = tempoSequencial/exp->tempoExecucao;
    exp->eficiencia = exp->aceleracao/numNucleos;
    exp->qtdThreads = M;
    exp->N = N;
    exp->tempoSequencial = tempoSequencial;
    extrairCsv(exp, "tempoConcorrente.csv");
    free(exp);

    return 0;
}

int main(int argc, char*argv[]) {

    //recebe os argumentos de entrada
    if(argc < 2) {
        fprintf(stderr, "Digite: ./main <QtdNumerosParaAnalisarPrimalidade> <QtdThreads>\n");
        return 2;
    }

    N = atoll(argv[1]);
    int M = atoi(argv[2]);

    //double inicio, fim, tempoTotal;
    //GET_TIME(inicio);
    //criarThreads(M);
    //GET_TIME(fim);
    //tempoTotal = fim - inicio;
    //printf("Tempo decorrido (Concorrente): %f segundos\nContagem de primalidade em %d números com %d threads\n", tempoTotal, N, M);

    geraResultados(M, N);

    /*
     * toDo:
        Roteiro
        1. Implemente uma versão concorrente para esse problema, dividindo a tarefa entre
        as threads. O número de elementos (N) e o número de threads devem ser informados na linha de comando.
        IMPORTANTE: Use o padrão de  bolsa de tarefa para distribuição da tarefa entre as threads: cada thread pega o próximo número
        da serie para avaliar a sua primalidade (veja Aula 4, de 16 de abril).
        ATENÇÃO:  os valores da série NÃO devem ser colocados em um vetor, devem  ser obtidos incrementalmente em tempo de execução.
        2. OBS.: defina a variável N do tipo long long int e use a função atoll() para converter o valor recebido do usuário (string) para long long int.
        3. Certifique-se que a sua solução está correta.
        4. Inclua tomadas de tempo no código (parte de processamento da sequência).
        5. Execute o programa várias vezes, variando o valor de N (experimente 10ˆ3
        e 10ˆ6);
        e para cada valor de N, varie a quantidade de threads (1, 2 e 4).
        6. Para cada configuração, execute o programa ao menos 5 vezes e registre o tempo médio dessas execuções.
        7. Calcule a aceleração e a eficiência  alcançada em cada configuração, tomando como base (execução sequencial) a execução com 1 thread.

     */
}