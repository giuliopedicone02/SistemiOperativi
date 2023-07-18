#include "../lib-misc.h"
#include "bst.h"
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

typedef enum
{
    BST_THREAD,
    PADRE
} n_thread;

typedef enum
{
    OPERAZIONE,
    BST_REF,
    VALORE
} op_spec;

typedef struct
{
    char operazione[50];
    bool done;
    sem_t sem[2]; // 0: Thread_n, 1: Thread padre
} shared;

typedef struct
{
    pthread_t tid;
    int t_names;
    char *filename;
    shared *shared;
} thread_data;

void init_shared(shared *sh)
{
    int err;

    memset(sh->operazione, 0, sizeof(sh->operazione));
    sh->done = 0;

    if ((err = sem_init(&sh->sem[BST_THREAD], 0, 0)) != 0) // Semaforo per i BST-Thread
    {
        exit_with_err("sem_init", err);
    }

    if ((err = sem_init(&sh->sem[PADRE], 0, 1)) != 0) // Semaforo per il Thread Padre
    {
        exit_with_err("sem_init", err);
    }
}

void destroy_shared(shared *sh)
{
    int err;
    if ((err = sem_destroy(&sh->sem[BST_THREAD])) != 0)
    {
        exit_with_err("sem_destroy", err);
    }

    if ((err = sem_destroy(&sh->sem[PADRE])) != 0)
    {
        exit_with_err("sem_destroy", err);
    }

    free(sh);
}

void *bst_thread(void *arg)
{
    int err;
    thread_data *sh = (thread_data *)arg;

    FILE *file = fopen(sh->filename, "r");

    if (!file)
    {
        printf("[THREAD-%d] Errore nell'apertura del file\n", sh->t_names);
        exit(EXIT_FAILURE);
    }

    int valore = 0;

    BST *bst = NULL;

    while (fscanf(file, "%d\n", &valore) == 1)
    {
        bst = insertNode(bst, valore);
    }

    while (1)
    {
        if ((err = sem_wait(&sh->shared->sem[BST_THREAD])) != 0)
        {
            exit_with_err("sem_wait", err);
        }

        if (sh->shared->done)
        {
            break;
        }

        char *token = strtok(sh->shared->operazione, " ");

        char operazione[3][10];
        int i = 0;

        while (token != NULL)
        {
            strcpy(operazione[i++], token);
            token = strtok(NULL, " ");
        }

        if (strcmp(operazione[OPERAZIONE], "add") == 0)
        {
            printf("Aggiunta del numero: %s\n", operazione[VALORE]);
            insertNode(bst, atoi(operazione[VALORE]));
        }
        else if (strcmp(operazione[OPERAZIONE], "search") == 0)
        {
            printf("Ricerca del numero: %s\n", operazione[VALORE]);
        }
        else if (strcmp(operazione[OPERAZIONE], "print") == 0)
        {
            printf("Stampa del BST: ");
            printInorder(bst);
            printf("\n");
        }
        else
        {
            printf("Operazione non consentita!\n");
        }

        sleep(3);

        if ((err = sem_post(&sh->shared->sem[PADRE])) != 0)
        {
            exit_with_err("sem_post", err);
        }
    }

    pthread_exit(NULL);
}

void *padre(void *arg)
{
    int err;
    thread_data *sh = (thread_data *)arg;

    while (1)
    {
        if ((err = sem_wait(&sh->shared->sem[PADRE])) != 0)
        {
            exit_with_err("sem_wait", err);
        }

        printf("Inserisci una delle seguenti operazioni: \n");
        printf("- add n key\n");
        printf("- search n key\n");
        printf("- print n\n");
        printf("- quit\n");
        printf("> ");

        char operazione[20] = {'\0'};

        fgets(operazione, sizeof(operazione), stdin);
        operazione[strlen(operazione) - 1] = '\0';

        if (strcmp(operazione, "quit") == 0)
        {
            exit(EXIT_SUCCESS);
        }

        strcpy(sh->shared->operazione, operazione);

        if ((err = sem_post(&sh->shared->sem[BST_THREAD])) != 0)
        {
            exit_with_err("sem_post", err);
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: %s <file1.txt> <file2.txt> ...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int numeroBST = argc - 1;
    int err;

    thread_data td[numeroBST + 1];
    shared *sh = malloc(sizeof(shared));

    init_shared(sh);

    // Creazione degli n-thread generici
    for (int i = 0; i < numeroBST; i++)
    {
        td[i].t_names = i + 1;
        td[i].filename = argv[i + 1];
        td[i].shared = sh;

        if ((err = pthread_create(&td[i].tid, NULL, (void *)bst_thread, (void *)&td[i])) != 0)
        {
            exit_with_err("pthread_create", err);
        }
    }

    // printf("Ho creato %d thread BST\n", numeroBST);

    // Creazione del thread padre
    td[numeroBST].shared = sh;
    if ((err = pthread_create(&td[numeroBST].tid, NULL, (void *)padre, (void *)&td[numeroBST])) != 0)
    {
        exit_with_err("pthread_create", err);
    }

    for (int i = 0; i <= numeroBST; i++)
    {
        if ((err = pthread_join(td[i].tid, NULL)) != 0)
        {
            exit_with_err("pthread_join", err);
        }
    }

    destroy_shared(sh);
    exit(EXIT_SUCCESS);
}