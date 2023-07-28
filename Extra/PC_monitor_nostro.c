#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#define N 10
#define LEN 100
#define handle(msg)                               \
    do                                            \
    {                                             \
        fprintf(stderr, "[%d] error ", __LINE__); \
        perror(msg);                              \
        exit(EXIT_FAILURE);                       \
    } while (false);

typedef struct
{
    char buffer[N][LEN];
    unsigned in, out, size, finish, n_producer;
    pthread_mutex_t lock;
    pthread_cond_t empty, full;
} Shared_prod;

typedef struct
{
    char word[LEN];
    bool turn, finish;
    pthread_mutex_t lock;
    pthread_cond_t consumer, main;
} Shared_main;

typedef struct
{
    pthread_t tid;
    unsigned ID;
    char *file;
    Shared_prod *sh_prod;
    Shared_main *sh_main;
} Thread;

void shared_prod_init(Shared_prod *sh, unsigned n_producer)
{
    for (int i = 0; i < N; i++)
        memset(sh->buffer[i], 0, LEN);
    sh->n_producer = n_producer;
    sh->in = sh->out = sh->size = sh->finish = 0;
    if (pthread_mutex_init(&sh->lock, NULL))
        handle("rwlock init");
    if (pthread_cond_init(&sh->full, NULL))
        handle("cond init");
    if (pthread_cond_init(&sh->empty, NULL))
        handle("cond init");
}

void shared_main_init(Shared_main *sh)
{
    sh->turn = sh->finish = 0; // inizia il consumer
    if (pthread_mutex_init(&sh->lock, NULL))
        handle("mutex init");
    if (pthread_cond_init(&sh->main, NULL))
        handle("cond init");
    if (pthread_cond_init(&sh->consumer, NULL))
        handle("cond init");
}

void shared_prod_destroy(Shared_prod *sh)
{
    if (pthread_mutex_destroy(&sh->lock))
        handle("rwlock destroy");
    if (pthread_cond_destroy(&sh->full))
        handle("cond destroy");
    if (pthread_cond_destroy(&sh->empty))
        handle("cond destroy");
    free(sh);
}

void shared_main_destroy(Shared_main *sh)
{
    if (pthread_mutex_destroy(&sh->lock))
        handle("mutex destroy");
    if (pthread_cond_destroy(&sh->main))
        handle("cond destroy");
    if (pthread_cond_destroy(&sh->consumer))
        handle("cond destroy");
    free(sh);
}

void inserisciElemento(char *word, Thread *th)
{
    if (pthread_mutex_lock(&th->sh_prod->lock))
        handle("lock");
    while (th->sh_prod->size == N)
        if (pthread_cond_wait(&th->sh_prod->full, &th->sh_prod->lock))
            handle("wait");

    printf("[Producer-%d] %s\n", th->ID, word);
    strcpy(th->sh_prod->buffer[th->sh_prod->in], word);
    th->sh_prod->in = (th->sh_prod->in + 1) % N;
    th->sh_prod->size++;

    if (th->sh_prod->size == 1)
        if (pthread_cond_signal(&th->sh_prod->empty))
            handle("signal");
    if (pthread_mutex_unlock(&th->sh_prod->lock))
        handle("unlock");
}

void Producer(void *arg)
{
    Thread *th = (Thread *)arg;
    FILE *fp;
    char line[LEN], *word;

    if (!(fp = fopen(th->file, "r")))
        handle("fopen");

    while (fgets(line, LEN, fp))
    {
        if (line[strlen(line) - 1] == '\n')
        {
            line[strlen(line) - 1] = '\0';
        }

        word = strtok(line, " ");
        while (word)
        {
            inserisciElemento(word, th);
            word = strtok(NULL, " ");
        }
    }

    /*if (pthread_mutex_lock(&th->sh_prod->lock))
        handle("lock");
    while (th->sh_prod->size == N)
        if (pthread_cond_wait(&th->sh_prod->full, &th->sh_prod->lock))
            handle("wait");

    th->sh_main->finish++;

    if (pthread_cond_signal(&th->sh_prod->empty))
        handle("signal");
    if (pthread_mutex_unlock(&th->sh_prod->lock))
        handle("unlock");
*/
    pthread_exit(NULL);
}

void stampaStringa(char *word, Thread *th)
{
    // printf("Ok\n");
    if (pthread_mutex_lock(&th->sh_main->lock))
        handle("lock");

    while (th->sh_main->turn != 0)
        if (pthread_cond_wait(&th->sh_main->consumer, &th->sh_main->lock))
            handle("wait");

    strcpy(th->sh_main->word, word);
    th->sh_main->turn = 1;

    if (pthread_cond_signal(&th->sh_main->main))
        handle("signal");
    if (pthread_mutex_unlock(&th->sh_main->lock))
        handle("unlock");
}

void estraiElemento(Thread *th)
{
    char word[LEN];

    if (pthread_mutex_lock(&th->sh_prod->lock))
        handle("lock");
    while (th->sh_prod->size == 0)
        if (pthread_cond_wait(&th->sh_prod->empty, &th->sh_prod->lock))
            handle("wait");

    /*if (th->sh_prod->finish == th->sh_prod->n_producer && th->sh_prod->size == 0)
    {
        if (pthread_cond_signal(&th->sh_main->main))
            handle("signal");
        if (pthread_mutex_unlock(&th->sh_prod->lock))
            handle("unlock");
        break;
    }*/

    strcpy(word, th->sh_prod->buffer[th->sh_prod->out]);
    th->sh_prod->out = (th->sh_prod->out + 1) % N;
    th->sh_prod->size--;

    printf("[CONSUMER] parola letta : %s\n", word);

    if (th->sh_prod->size == N - 1)
        if (pthread_cond_signal(&th->sh_prod->full))
            handle("signal");
    if (pthread_mutex_unlock(&th->sh_prod->lock))
        handle("unlock");

    stampaStringa(word, th);
}

void Consumer(void *arg)
{
    Thread *th = (Thread *)arg;
    char *word;

    while (true)
    {
        estraiElemento(th);
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
        handle("argc");
    int n_producer = argc - 1;

    Thread th[n_producer + 1];
    Shared_prod *sh_prod = malloc(sizeof(Shared_prod));
    Shared_main *sh_main = malloc(sizeof(Shared_main));
    shared_prod_init(sh_prod, n_producer);
    shared_main_init(sh_main);

    // Producer
    for (int i = 0; i < n_producer; i++)
    {
        th[i].ID = i + 1;
        th[i].file = argv[i + 1];
        th[i].sh_prod = sh_prod;
        if (pthread_create(&th[i].tid, NULL, (void *)Producer, (void *)&th[i]))
            handle("create");
    }

    // Consumer
    th[n_producer].sh_prod = sh_prod;
    th[n_producer].sh_main = sh_main;
    if (pthread_create(&th[n_producer].tid, NULL, (void *)Consumer, (void *)&th[n_producer]))
        handle("create");

    // Main
    while (true)
    {
        if (pthread_mutex_lock(&sh_main->lock))
            handle("lock");
        while (sh_main->turn != 1)
            if (pthread_cond_wait(&sh_main->main, &sh_main->lock))
                handle("wait");

        if (sh_main->finish)
        {
            if (pthread_mutex_unlock(&sh_main->lock))
                handle("unlock");
            break;
        }

        printf("[MAIN] %s\n", sh_main->word);
        sh_main->turn = 0;
        if (pthread_cond_signal(&sh_main->consumer))
            handle("signal");
        if (pthread_mutex_unlock(&sh_main->lock))
            handle("unlock");
    }

    for (int i = 0; i < n_producer + 1; i++)
        if (pthread_join(th[i].tid, NULL))
            handle("join");

    shared_prod_destroy(sh_prod);
    shared_main_destroy(sh_main);
    exit(EXIT_SUCCESS);
}