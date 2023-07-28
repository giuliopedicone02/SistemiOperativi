#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define handle(msg)                               \
    do                                            \
    {                                             \
        fprintf(stderr, "[%d] error ", __LINE__); \
        perror(msg);                              \
        exit(EXIT_FAILURE);                       \
    } while (false);

#define BUFFER_SIZE 10
#define NUM_PRODUCERS 5
#define NUM_CONSUMERS 5
#define NUM_INSERTION 15

typedef struct
{
    int *buffer, in, out;
    unsigned size, capacity;
} Buffer;

typedef struct
{
    Buffer *buffer;
    bool finish;
    unsigned insertion;
    sem_t empty, full;
    pthread_rwlock_t rwlock;
} Shared;

typedef struct
{
    pthread_t tid;
    unsigned ID;
    Shared *sh;
} Thread;

void create_buffer(Buffer **buffer, int capacity)
{
    *buffer = malloc(sizeof(Buffer));
    (*buffer)->buffer = malloc(capacity * sizeof(int));
    (*buffer)->in = (*buffer)->out = (*buffer)->size = 0;
    (*buffer)->capacity = capacity;
}

void insert(Buffer *buffer, int value)
{
    buffer->buffer[buffer->in] = value;
    buffer->in = (buffer->in + 1) % buffer->capacity;
    buffer->size++;
}

int extract(Buffer *buffer)
{
    int value = buffer->buffer[buffer->out];
    buffer->out = (buffer->out + 1) % buffer->capacity;
    buffer->size--;
    return value;
}

void destroy_buffer(Buffer *buffer)
{
    free(buffer->buffer);
    free(buffer);
}

void shared_init(Shared *sh, int capacity)
{
    sh->insertion = 0;
    sh->finish = false;
    if (sem_init(&sh->empty, 0, capacity))
        handle("sem init");
    if (sem_init(&sh->full, 0, 0))
        handle("sem init");
    if (pthread_rwlock_init(&sh->rwlock, NULL))
        handle("rw_lock init");
    create_buffer(&sh->buffer, capacity);
}

void shared_destroy(Shared *sh)
{
    if (sem_destroy(&sh->empty))
        handle("sem destroy");
    if (sem_destroy(&sh->full))
        handle("sem destroy");
    if (pthread_rwlock_destroy(&sh->rwlock))
        handle("rw_lock destroy");
    destroy_buffer(sh->buffer);
}

void Producer(void *arg)
{
    Thread *th = (Thread *)arg;
    while (true)
    {
        int item = rand() % 100;

        if (sem_wait(&th->sh->empty))
            handle("wait");
        if (pthread_rwlock_wrlock(&th->sh->rwlock))
            handle("wr_lock");

        if (th->sh->insertion == NUM_INSERTION)
        {
            th->sh->finish = true;
            if (pthread_rwlock_unlock(&th->sh->rwlock))
                handle("unlock");
            if (sem_post(&th->sh->full))
                handle("post");
            break;
        }

        insert(th->sh->buffer, item);
        th->sh->insertion++;

        if (pthread_rwlock_unlock(&th->sh->rwlock))
            handle("unlock");
        if (sem_post(&th->sh->full))
            handle("post");

        printf("[Producer-%d] inserted: %d\n", th->ID, item);
    }
    pthread_exit(NULL);
}

void Consumer(void *arg)
{
    Thread *th = (Thread *)arg;
    while (true)
    {
        if (sem_wait(&th->sh->full))
            handle("wait");
        if (pthread_rwlock_rdlock(&th->sh->rwlock))
            handle("rd_lock");

        if (th->sh->finish)
        {
            if (pthread_rwlock_unlock(&th->sh->rwlock))
                handle("unlock");
            if (sem_post(&th->sh->empty))
                handle("post");
            break;
        }

        int item = extract(th->sh->buffer);

        if (pthread_rwlock_unlock(&th->sh->rwlock))
            handle("unlock");
        if (sem_post(&th->sh->empty))
            handle("post");

        printf("[Consumer-%d] extracted: %d\n", th->ID, item);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    srand(time(NULL));

    unsigned capacity;

    if (argc >= 2 && atoi(argv[1]) > 1)
        capacity = atoi(argv[1]);
    else
        capacity = BUFFER_SIZE;

    Shared *sh = malloc(sizeof(Shared));
    Thread producers[NUM_PRODUCERS];
    Thread consumers[NUM_CONSUMERS];
    shared_init(sh, capacity);

    for (int i = 0; i < NUM_PRODUCERS; i++)
    {
        producers[i].ID = i + 1;
        producers[i].sh = sh;
        if (pthread_create(&producers[i].tid, NULL, (void *)Producer, (void *)&producers[i]))
            handle("create");
    }

    for (int i = 0; i < NUM_CONSUMERS; i++)
    {
        consumers[i].ID = i + 1;
        consumers[i].sh = sh;
        if (pthread_create(&consumers[i].tid, NULL, (void *)Consumer, (void *)&consumers[i]))
            handle("create");
    }

    for (int i = 0; i < NUM_PRODUCERS; i++)
        if (pthread_join(producers[i].tid, NULL))
            handle("join producers");

    for (int i = 0; i < NUM_CONSUMERS; i++)
        if (pthread_join(consumers[i].tid, NULL))
            handle("join consumers");

    printf("\nNumber insertion: %d\n", sh->insertion);

    shared_destroy(sh);
    exit(EXIT_SUCCESS);
}