/*
 * ex1_paralelo.c - Exercicio 1: Paralelizacao e speedup
 *
 * Objetivo: paralelizar a contagem de numeros primos no intervalo
 * [2, N], dividindo o trabalho entre T threads. Cada thread conta os
 * primos do seu pedaco e guarda o resultado PARCIAL na sua struct de
 * argumentos. O main soma os parciais apos os joins.
 *
 * NESTA VERSAO NAO HA VARIAVEL COMPARTILHADA: cada thread escreve
 * apenas na propria struct. Por isso nao precisamos de mutex ainda.
 *
 * Compilar: gcc -Wall -O2 -o ex1_paralelo ex1_paralelo.c -pthread -lm
 * Executar: ./ex1_paralelo <N> <num_threads>
 * Exemplo:  ./ex1_paralelo 20000000 4
 *
 * Resultados esperados (para conferir se seu codigo esta correto):
 *   N = 5000000  ->  348513 primos
 *   N = 20000000 -> 1270607 primos
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

/* Argumentos e resultado parcial de cada thread */
typedef struct {
    long inicio;      /* primeiro numero do intervalo da thread (inclusive) */
    long fim;         /* ultimo numero do intervalo da thread (exclusive)   */
    long parcial;     /* quantos primos a thread encontrou                  */
} args_t;

/* Teste de primalidade simples (proposital, para gerar carga de CPU) */
int eh_primo(long n)
{
    if (n < 2) return 0;
    if (n % 2 == 0) return n == 2;
    for (long i = 3; i * i <= n; i += 2)
        if (n % i == 0) return 0;
    return 1;
}

/* Relogio de parede em segundos (para medir tempo) */
double agora(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

/* ============================================================== */
/* TODO (a): implemente a funcao executada por cada thread.        */
/*                                                                 */
/* Ela deve:                                                       */
/*   1. converter o argumento para (args_t *)                      */
/*   2. contar os primos no intervalo [inicio, fim)                */
/*   3. guardar a contagem no campo 'parcial' da struct            */
/*   4. retornar NULL                                              */
/* ============================================================== */
void *conta_primos(void *arg)
{
    args_t *a = (args_t *) arg;
    long count = 0;

    for (long n = a->inicio; n < a->fim; n++)
        if (eh_primo(n))
            count++;

    a->parcial = count;
    return NULL;
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "uso: %s <N> <num_threads>\n", argv[0]);
        return EXIT_FAILURE;
    }
    long N = atol(argv[1]);
    int T = atoi(argv[2]);

    pthread_t *threads = malloc(T * sizeof(pthread_t));
    args_t    *args    = malloc(T * sizeof(args_t));

    double t0 = agora();

    /* ============================================================== */
    /* TODO (b): divida o intervalo [2, N] em T pedacos de tamanho     */
    /* aproximadamente igual e crie uma thread para cada pedaco.       */
    /*                                                                 */
    /* Dica: tam = (N - 2) / T;                                        */
    /*       thread i cuida de [2 + i*tam, 2 + (i+1)*tam),             */
    /*       e a ultima thread vai ate N (para nao perder o resto).    */
    /* ============================================================== */

    long tam = (N - 2) / T;
    for (int i = 0; i < T; i++) {
        args[i].inicio = 2 + i * tam;
        args[i].fim    = (i == T - 1) ? (N + 1) : (2 + (i + 1) * tam);
        args[i].parcial = 0;
        if (pthread_create(&threads[i], NULL, conta_primos, &args[i]) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    /* ============================================================== */
    /* TODO (c): espere todas as threads (pthread_join) e some os      */
    /* resultados parciais em 'total'.                                 */
    /* ============================================================== */
    long total = 0;

    for (int i = 0; i < T; i++) {
        pthread_join(threads[i], NULL);
        total += args[i].parcial;
    }

    double t1 = agora();

    printf("N = %ld | threads = %d\n", N, T);
    printf("primos encontrados: %ld\n", total);
    printf("tempo: %.3f s\n", t1 - t0);

    free(threads);
    free(args);
    return 0;
}
