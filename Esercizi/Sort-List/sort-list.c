#include "../lib-misc.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <strings.h>
#include <semaphore.h>
#include <string.h>
#include <stdbool.h>
#define MAX_WORD_LEN 50

typedef enum
{
    P,
    S,
    C
} tnames;

typedef struct
{
    char parola1[MAX_WORD_LEN];
    char parola2[MAX_WORD_LEN];
    int risultato;
    bool done;
    sem_t sem[3];
} shared;

typedef struct
{
    pthread_t tid;
    tnames tnames;
    char file[1024];
    shared *sh;
} thread_data;

void init_shared(shared *sh)
{
    int err;
    sh->parola1[0] = '\0';
    sh->parola2[0] = '\0';
    sh->risultato = 0;
    sh->done = false;

    if ((err = sem_init(&sh->sem[P], 0, 0)) != 0)
    {
        exit_with_err("sem_init", err);
    }

    if ((err = sem_init(&sh->sem[S], 0, 1)) != 0)
    {
        exit_with_err("sem_init", err);
    }

    if ((err = sem_init(&sh->sem[C], 0, 0)) != 0)
    {
        exit_with_err("sem_init", err);
    }
}

void destroy_shared(shared *sh)
{
    int err;

    for (int i = 0; i < 3; i++)
    {
        if ((err = sem_destroy(&sh->sem[i])) != 0)
        {
            exit_with_err("sem_destroy", err);
        }
    }

    free(sh);
}

void swap(char *xp, char *yp)
{
    char temp[MAX_WORD_LEN];
    strcpy(temp, xp);
    strcpy(xp, yp);
    strcpy(yp, temp);
}

void Padre(void *arg)
{
    int err;
    thread_data *td = (thread_data *)arg;

    while (1)
    {
        if ((err = sem_wait(&td->sh->sem[P])) != 0)
        {
            exit_with_err("sem_wait", err);
        }

        if (td->sh->done)
        {
            break;
        }

        printf("%s\n", td->sh->parola1);

        if ((err = sem_post(&td->sh->sem[S])) != 0)
        {
            exit_with_err("sem_post", err);
        }
    }

    pthread_exit(NULL);
}

void Sorter(void *arg)
{
    int err;
    int N = 0;
    int c = 0;
    char **parole;
    char buffer[MAX_WORD_LEN];
    thread_data *td = (thread_data *)arg;

    FILE *fp = fopen(td->file, "r");
    if (!fp)
    {
        printf("Errore file!\n");
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, MAX_WORD_LEN, fp))
    {
        N++;
    }

    parole = (char **)malloc(N * sizeof(char *));

    for (int i = 0; i < N; i++)
    {
        parole[i] = (char *)malloc(MAX_WORD_LEN * sizeof(char));
    }

    rewind(fp);

    while (fgets(buffer, MAX_WORD_LEN, fp))
    {
        if (buffer[strlen(buffer) - 1] == '\n')
        {
            buffer[strlen(buffer) - 1] = '\0';
        }

        strcpy(parole[c], buffer);
        c++;
    }

    for (int i = 0; i < N - 1; i++)
    {
        for (int j = 0; j < N - i - 1; j++)
        {
            if ((err = sem_wait(&td->sh->sem[S])) != 0)
            {
                exit_with_err("sem_wait", err);
            }

            strcpy(td->sh->parola1, parole[j]);
            strcpy(td->sh->parola2, parole[j + 1]);

            if ((err = sem_post(&td->sh->sem[C])) != 0)
            {
                exit_with_err("sem_post", err);
            }

            if ((err = sem_wait(&td->sh->sem[S])) != 0)
            {
                exit_with_err("sem_wait", err);
            }

            if (td->sh->risultato > 0)
            {
                swap(parole[j], parole[j + 1]);
            }

            if ((err = sem_post(&td->sh->sem[C])) != 0)
            {
                exit_with_err("sem_post", err);
            }
        }
    }

    for (int i = 0; i < N; i++)
    {
        if ((err = sem_wait(&td->sh->sem[S])) != 0)
        {
            exit_with_err("sem_wait", err);
        }

        strcpy(td->sh->parola1, parole[i]);

        if ((err = sem_post(&td->sh->sem[P])) != 0)
        {
            exit_with_err("sem_post", err);
        }
    }

    if ((err = sem_wait(&td->sh->sem[S])) != 0)
    {
        exit_with_err("sem_wait", err);
    }

    td->sh->done = true;

    if ((err = sem_post(&td->sh->sem[C])) != 0)
    {
        exit_with_err("sem_post", err);
    }

    if ((err = sem_post(&td->sh->sem[P])) != 0)
    {
        exit_with_err("sem_post", err);
    }

    // Deallocazione della memoria
    for (int i = 0; i < N; i++)
    {
        free(parole[i]);
    }

    free(parole);

    pthread_exit(NULL);
}

void Comparer(void *arg)
{
    int err;
    thread_data *td = (thread_data *)arg;

    while (1)
    {
        if ((err = sem_wait(&td->sh->sem[C])) != 0)
        {
            exit_with_err("sem_wait", err);
        }

        if (td->sh->done)
        {
            break;
        }

        td->sh->risultato = strcasecmp(td->sh->parola1, td->sh->parola2);

        if ((err = sem_post(&td->sh->sem[S])) != 0)
        {
            exit_with_err("sem_post", err);
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    int err;

    if (argc != 2)
    {
        printf("Usare %s, nomefile,txt", argv[0]);
        exit(EXIT_FAILURE);
    }

    thread_data td[3];
    shared *st = malloc(sizeof(shared));

    init_shared(st);

    for (int i = 0; i < 3; i++)
    {
        td[i].tnames = i;
        strcpy(td[i].file, argv[1]);
        td[i].sh = st;
    }

    if ((err = pthread_create(&td[P].tid, NULL, (void *)Padre, (void *)&td[P])) != 0)
    {
        exit_with_err("pthread_create", err);
    }

    if ((err = pthread_create(&td[S].tid, NULL, (void *)Sorter, (void *)&td[S])) != 0)
    {
        exit_with_err("pthread_create", err);
    }

    if ((err = pthread_create(&td[C].tid, NULL, (void *)Comparer, (void *)&td[C])) != 0)
    {
        exit_with_err("pthread_create", err);
    }

    for (int i = 0; i < 3; i++)
    {
        if ((err = pthread_join(td[i].tid, NULL)) != 0)
        {
            exit_with_err("pthread_join", err);
        }
    }

    exit(EXIT_SUCCESS);
}
