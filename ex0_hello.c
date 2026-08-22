/*
 * ex0_hello.c - Exemplo COMPLETO de aquecimento (nao precisa modificar)
 *
 * Demonstra os conceitos basicos da API pthreads:
 *   - pthread_create: cria uma nova thread
 *   - pthread_join:   espera uma thread terminar
 *   - passagem de argumentos via struct
 *
 * Compilar: gcc -Wall -O2 -o ex0_hello ex0_hello.c -pthread
 * Executar: ./ex0_hello
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 4

/* Struct usada para passar argumentos para cada thread.
 * Como pthread_create aceita apenas UM ponteiro (void *),
 * agrupamos tudo que a thread precisa em uma struct. */
typedef struct {
    int id;          /* identificador logico da thread   */
    int valor;       /* um dado qualquer para demonstrar */
} args_t;

/* Funcao executada por cada thread.
 * Assinatura obrigatoria: void *funcao(void *arg) */
void *trabalho(void *arg)
{
    args_t *a = (args_t *) arg;

    printf("Ola da thread %d! Recebi o valor %d.\n", a->id, a->valor);

    /* O valor retornado pode ser recuperado pelo pthread_join.
     * Aqui devolvemos o dobro do valor recebido. */
    a->valor = a->valor * 2;
    return (void *) a;
}

int main(void)
{
    pthread_t threads[NUM_THREADS];
    args_t args[NUM_THREADS];

    /* 1. Criacao: dispara NUM_THREADS threads em paralelo */
    for (int i = 0; i < NUM_THREADS; i++) {
        args[i].id = i;
        args[i].valor = (i + 1) * 10;
        if (pthread_create(&threads[i], NULL, trabalho, &args[i]) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    /* 2. Espera: pthread_join bloqueia ate a thread i terminar.
     * Sem o join, o main poderia terminar antes das threads! */
    for (int i = 0; i < NUM_THREADS; i++) {
        void *retorno;
        pthread_join(threads[i], &retorno);
        args_t *r = (args_t *) retorno;
        printf("main: thread %d terminou e retornou %d\n", r->id, r->valor);
    }

    printf("main: todas as threads terminaram.\n");
    return 0;
}
