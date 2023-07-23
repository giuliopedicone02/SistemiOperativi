#include "../lib-misc.h"
#include <stdlib.h>
#include <semaphore.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>

typedef enum
{
    MNG,
    ADD,
    MUL,
    SUB
} tnames;

typedef struct
{
    long int parziale;
    long int valore;
    bool done;
    sem_t sem[4];
} shared;

typedef struct
{
    char file[1024];
    pthread_t tid;
    tnames tnames;
    shared *sh;
} thread_data;

void init_shared(shared *sh)
{
    int err;
    sh->done = false;
    sh->parziale = 0;
    sh->valore = 0;

    if ((err = sem_init(&sh->sem[MNG], 0, 1)) != 0)
    {
        exit_with_err("sem_init", err);
    }

    if ((err = sem_init(&sh->sem[ADD], 0, 0)) != 0)
    {
        exit_with_err("sem_init", err);
    }

    if ((err = sem_init(&sh->sem[MUL], 0, 0)) != 0)
    {
        exit_with_err("sem_init", err);
    }

    if ((err = sem_init(&sh->sem[SUB], 0, 0)) != 0)
    {
        exit_with_err("sem_init", err);
    }
}

void destroy_shared(shared *sh)
{
    int err;
    for (int i = 0; i < 4; i++)
    {
        if ((err = sem_destroy(&sh->sem[i])) != 0)
        {
            exit_with_err("sem_destroy", err);
        }
    }

    free(sh);
}

void Manager(void *arg)
{
    int err;
    thread_data *td = (thread_data *)arg;
    char buffer[1024];

    FILE *fp = fopen(td->file, "r");

    if (!fp)
    {
        printf("Errore lettura del file: %s\n", td->file);
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, 1024, fp))
    {
        if (buffer[strlen(buffer) - 1] == '\n')
        {
            buffer[strlen(buffer) - 1] = '\0';
        }

        if ((err = sem_wait(&td->sh->sem[MNG])) != 0)
        {
            exit_with_err("sem_wait", err);
        }

        printf("[MNG]: risultato intermedio: %d; letto \"%s\"\n", td->sh->parziale, buffer);

        td->sh->valore = atoi(&buffer[1]);

        switch (buffer[0])
        {
        case '+':
            if ((err = sem_post(&td->sh->sem[ADD])) != 0)
            {
                exit_with_err("sem_post", err);
            }
            break;
        case '*':
            if ((err = sem_post(&td->sh->sem[MUL])) != 0)
            {
                exit_with_err("sem_post", err);
            }
            break;
        case '-':
            if ((err = sem_post(&td->sh->sem[SUB])) != 0)
            {
                exit_with_err("sem_post", err);
            }
            break;

        default:
            printf("Carattere %c non ammesso! Utillizare solo (+,-,*)\n", buffer[0]);
            exit(EXIT_FAILURE);
            break;
        }
    }

    if ((err = sem_wait(&td->sh->sem[MNG])) != 0)
    {
        exit_with_err("sem_wait", err);
    }

    printf("[MNG]: risultato finale: %d\n", td->sh->parziale);
    td->sh->done = true;

    if ((err = sem_post(&td->sh->sem[ADD])) != 0)
    {
        exit_with_err("sem_post", err);
    }

    if ((err = sem_post(&td->sh->sem[MUL])) != 0)
    {
        exit_with_err("sem_post", err);
    }

    if ((err = sem_post(&td->sh->sem[SUB])) != 0)
    {
        exit_with_err("sem_post", err);
    }

    pthread_exit(NULL);
}

void Somma(void *arg)
{
    int err;
    thread_data *td = (thread_data *)arg;

    while (1)
    {
        if ((err = sem_wait(&td->sh->sem[ADD])) != 0)
        {
            exit_with_err("sem_wait", err);
        }

        if (td->sh->done)
        {
            break;
        }

        printf("[ADD]: %d+%d=%d\n", td->sh->parziale, td->sh->valore, td->sh->parziale + td->sh->valore);
        td->sh->parziale = td->sh->parziale + td->sh->valore;

        if ((err = sem_post(&td->sh->sem[MNG])) != 0)
        {
            exit_with_err("sem_post", err);
        }
    }

    pthread_exit(NULL);
}

void Moltiplicazione(void *arg)
{
    int err;
    thread_data *td = (thread_data *)arg;

    while (1)
    {
        if ((err = sem_wait(&td->sh->sem[MUL])) != 0)
        {
            exit_with_err("sem_wait", err);
        }

        if (td->sh->done)
        {
            break;
        }

        printf("[MUL]: %d*%d=%d \n", td->sh->parziale, td->sh->valore, td->sh->parziale * td->sh->valore);
        td->sh->parziale = td->sh->parziale * td->sh->valore;

        if ((err = sem_post(&td->sh->sem[MNG])) != 0)
        {
            exit_with_err("sem_post", err);
        }
    }

    pthread_exit(NULL);
}

void Sottrazione(void *arg)
{
    int err;
    thread_data *td = (thread_data *)arg;

    while (1)
    {
        if ((err = sem_wait(&td->sh->sem[SUB])) != 0)
        {
            exit_with_err("sem_wait", err);
        }

        if (td->sh->done)
        {
            break;
        }

        printf("[SUB]: %d-%d=%d \n", td->sh->parziale, td->sh->valore, td->sh->parziale - td->sh->valore);
        td->sh->parziale = td->sh->parziale - td->sh->valore;

        if ((err = sem_post(&td->sh->sem[MNG])) != 0)
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
        printf("Usage: %s <file.txt>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    thread_data td[4];
    shared *st = malloc(sizeof(shared));

    init_shared(st);

    strcpy(td[0].file, argv[1]);

    for (int i = 0; i < 4; i++)
    {
        td[i].tnames = i;
        td[i].sh = st;
    }

    if ((err = pthread_create(&td[MNG].tid, NULL, (void *)Manager, (void *)&td[MNG])) != 0)
    {
        exit_with_err("pthread_create", err);
    }

    if ((err = pthread_create(&td[ADD].tid, NULL, (void *)Somma, (void *)&td[ADD])) != 0)
    {
        exit_with_err("pthread_create", err);
    }

    if ((err = pthread_create(&td[MUL].tid, NULL, (void *)Moltiplicazione, (void *)&td[MUL])) != 0)
    {
        exit_with_err("pthread_create", err);
    }

    if ((err = pthread_create(&td[SUB].tid, NULL, (void *)Sottrazione, (void *)&td[SUB])) != 0)
    {
        exit_with_err("pthread_create", err);
    }

    for (int i = 0; i < 4; i++)
    {
        if ((err = pthread_join(td[i].tid, NULL)) != 0)
        {
            exit_with_err("pthread_join", err);
        }
    }

    destroy_shared(st);

    exit(EXIT_SUCCESS);
}