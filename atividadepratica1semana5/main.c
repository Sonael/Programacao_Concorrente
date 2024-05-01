/*
https://docs.google.com/presentation/d/1UbXLo1MGNND40tFzzHByNh7Cgn6FZBgI1v0rKktHnGs/edit#slide=id.g1f7b1abbe14_0_168

Neste exercício, temos três threads produtoras e uma thread consumidora.
As threads produtoras geram números aleatórios e os adicionam à fila.
A thread consumidora remove os números da fila.
As variáveis de condição são usadas para garantir que as threads produtoras
esperem se a fila estiver cheia e que a thread consumidora espere se a fila estiver vazia.
O mutex é usado para garantir o acesso seguro à fila compartilhada entre as threads.

*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define QUEUE_SIZE 5
#define NUM_PRODUCERS 3

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_not_empty = PTHREAD_COND_INITIALIZER;

int queue = 0;
int queue_count = QUEUE_SIZE;
int next_producer_id = 0;

void *uso_caixa(void *arg)
{

    queue++;
    printf("Existe uma pessoa utilizando o caixa, por favor aguarde.\n");

    sleep(rand() % 3); // Espera um tempo aleatório antes de adicionar outro elemento
    queue--;
}

void *espera_na_fila(void *arg)
{
    int producer_id = *((int *)arg);
    pthread_mutex_lock(&mutex);
    printf("Existem %d pessoas na fila.\n", queue_count);
    if (queue == 0)
    {
        printf("Não há ninguém utilizando o caixa\n");
        uso_caixa(NULL);
    }

    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main()
{
    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumer;

    srand(time(NULL));

    for (int i = 0; i < QUEUE_SIZE; i++)
    {
        int *producer_id = malloc(sizeof(int));
        *producer_id = i + 1;
        pthread_create(&producers[i], NULL, espera_na_fila, producer_id);
    }

    for (int i = 0; i < QUEUE_SIZE; i++)
    {
        pthread_join(producers[i], NULL);
    }

    return 0;
}