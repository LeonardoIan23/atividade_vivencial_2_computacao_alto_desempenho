/*
 * ex2_corrida.c - Exercicio 2: Condicao de corrida e mutex
 *
 * Este programa simula um sistema de caixa de banco: T threads
 * processam, cada uma, DEPOSITOS_POR_THREAD depositos de R$ 1 em uma
 * MESMA conta compartilhada (variavel global 'saldo').
 *
 * PARTE (a): compile e execute o programa como esta. O saldo final
 * deveria ser T * DEPOSITOS_POR_THREAD, mas... execute varias vezes
 * e observe. Por que o resultado esta errado e muda a cada execucao?
 *
 * PARTE (b): proteja a secao critica com um pthread_mutex_t para
 * corrigir o resultado. Meca o tempo.
 *
 * PARTE (c): a versao (b) fica correta, porem lenta: o mutex e
 * disputado milhoes de vezes. Reescreva usando um acumulador LOCAL
 * em cada thread e travando o mutex UMA unica vez ao final, para
 * somar o parcial ao saldo global. Compare os tempos de (b) e (c).
 *
 * Compilar: gcc -Wall -O2 -o ex2_corrida ex2_corrida.c -pthread
 * Executar: ./ex2_corrida <num_threads>
 * Exemplo:  ./ex2_corrida 4
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define DEPOSITOS_POR_THREAD 5000000L

/* Conta bancaria compartilhada por todas as threads.
 *
 * Nota: 'volatile' esta aqui apenas para impedir que o compilador
 * otimize os acessos a 'saldo' e esconda o problema. ATENCAO:
 * volatile NAO corrige condicoes de corrida -- quem faz isso e o
 * mutex que voce vai adicionar! */
volatile long saldo = 0;

/* TODO (b): declare aqui um mutex global.
 * Dica: pthread_mutex_t trava = PTHREAD_MUTEX_INITIALIZER; */
pthread_mutex_t trava = PTHREAD_MUTEX_INITIALIZER;

double agora(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

/* Cada thread deposita R$ 1, DEPOSITOS_POR_THREAD vezes.
 *
 * ATENCAO: 'saldo++' parece uma operacao unica, mas o processador a
 * executa em tres passos (ler saldo -> somar 1 -> escrever saldo).
 * Duas threads podem intercalar esses passos e perder depositos!
 */
void *caixa(void *arg)
{
    (void) arg; /* nao usamos argumentos neste exercicio */

    /* TODO (b): proteja esta secao critica com o mutex.        */
    /* TODO (c): depois, troque por um acumulador local e mova  */
    /*           o mutex para fora do laco.                     */
    long local = 0;
    for (long i = 0; i < DEPOSITOS_POR_THREAD; i++)
        local++;

    pthread_mutex_lock(&trava);
    saldo += local;
    pthread_mutex_unlock(&trava);

    return NULL;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "uso: %s <num_threads>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int T = atoi(argv[1]);

    pthread_t *threads = malloc(T * sizeof(pthread_t));

    double t0 = agora();

    for (int i = 0; i < T; i++) {
        if (pthread_create(&threads[i], NULL, caixa, NULL) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < T; i++)
        pthread_join(threads[i], NULL);

    double t1 = agora();

    long esperado = (long) T * DEPOSITOS_POR_THREAD;
    printf("threads = %d\n", T);
    printf("saldo esperado: %ld\n", esperado);
    printf("saldo obtido:   %ld  %s\n", saldo,
           saldo == esperado ? "(correto)" : "(ERRADO!)");
    printf("tempo: %.3f s\n", t1 - t0);

    free(threads);
    return 0;
}
