#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_INGRESSOS 10
#define NUM_PESSOAS 15
#define NUM_GERADOR_INGRESSOS 20

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_not_empty = PTHREAD_COND_INITIALIZER;
int ingressos_disponiveis = NUM_INGRESSOS;

void *compra_ingresso(void *thread_id)
{

    int tid = *((int *)thread_id);
    while (1)
    {
        pthread_mutex_lock(&mutex);

        while (ingressos_disponiveis <= 0)
        {
            printf("Pessoa %d: Desculpe, não há mais ingressos disponíveis.\n | Quantidade atual de Ingressos = %d.\n", tid, ingressos_disponiveis);
            pthread_cond_wait(&cond_not_empty, &mutex);
        }

        ingressos_disponiveis--;

        printf("Pessoa %d: Comprou ingresso. | Quantidade atual de Ingressos = %d.\n", tid, ingressos_disponiveis);

        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond_not_full);

        sleep(1); // Espera um tempo aleatório antes de remover outro elemento
    }

    pthread_exit(NULL);
}

void *gerar_ingressos(void *thread_id)
{
    int tid = *((int *)thread_id);

    while (1)
    {
        pthread_mutex_lock(&mutex);
        while (ingressos_disponiveis == NUM_INGRESSOS)
        {
            printf("Gerador de ingressos %d: Quantidade Maxima de Ingressos Atingida. | Quantidade atual de Ingressos = %d.\n", tid, ingressos_disponiveis);
            pthread_cond_wait(&cond_not_full, &mutex);
        }

        ingressos_disponiveis++;

        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond_not_empty);
        sleep(1);
    }

    pthread_exit(NULL);
}

int main()
{
    pthread_t threads[NUM_PESSOAS];
    int thread_ids[NUM_PESSOAS];

    pthread_t ingresso_threads[NUM_GERADOR_INGRESSOS];
    int ingresso_thread_ids[NUM_GERADOR_INGRESSOS];

    for (int i = 0; i < NUM_PESSOAS; i++)
    {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, compra_ingresso, (void *)&thread_ids[i]);
    }

    for (int i = 0; i < NUM_GERADOR_INGRESSOS; i++)
    {
        ingresso_thread_ids[i] = i;
        pthread_create(&ingresso_threads[i], NULL, gerar_ingressos, (void *)&ingresso_thread_ids[i]);
    }

    for (int i = 0; i < NUM_PESSOAS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < NUM_GERADOR_INGRESSOS; i++)
    {
        pthread_join(ingresso_threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}