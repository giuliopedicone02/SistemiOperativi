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
    INSERIMENTO,
    RICERCA,
    STAMPA
} operazione;

typedef struct
{
    operazione operazione;
    int numeroBST;
    int valore;
    bool done;
    sem_t *sem;
} shared;

typedef struct
{
    pthread_t tid;
    int t_names;
    int n_thread;
    char *filename;
    shared *shared;
} thread_data;

void init_shared(shared *sh, int numeroThread)
{
    int err;

    sh->operazione = -1;
    sh->numeroBST = -1;
    sh->valore = -1;
    sh->done = 0;

    for (int i = 0; i < numeroThread; i++)
    {
        if ((err = sem_init(&sh->sem[i], 0, 0)) != 0) // Semafori per i BST-Thread
        {
            exit_with_err("sem_init", err);
        }
    }

    if ((err = sem_init(&sh->sem[numeroThread], 0, 1)) != 0) // Semaforo per il Thread Padre
    {
        exit_with_err("sem_init", err);
    }
}

void destroy_shared(shared *sh, int numeroThread)
{
    int err;

    for (int i = 0; i < numeroThread + 1; i++)
    {
        if ((err = sem_destroy(&sh->sem[i])) != 0)
        {
            exit_with_err("sem_destroy", err);
        }
    }

    free(sh);
}

void bst_thread(void *arg)
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
        if ((err = sem_wait(&sh->shared->sem[sh->t_names])) != 0)
        {
            exit_with_err("sem_wait", err);
        }

        if (sh->shared->done)
        {
            break;
        }

        if (sh->shared->operazione == INSERIMENTO)
        {
            printf("\nHo inserito il valore %d nel BST %d\n", sh->shared->valore, sh->shared->numeroBST);
            insertNode(bst, sh->shared->valore);
        }
        else if (sh->shared->operazione == RICERCA)
        {
            printf("\nRicerca del valore %d nel BST %d in corso...\n", sh->shared->valore, sh->shared->numeroBST);
            if (search(bst, sh->shared->valore) == NULL)
            {
                printf("NON ho trovato il valore %d\n", sh->shared->valore);
            }
            else
            {
                printf("Ho trovato il valore %d\n", sh->shared->valore);
            }
        }
        else if (sh->shared->operazione == STAMPA)
        {
            printf("\nStampa del BST %d: ", sh->shared->numeroBST);
            printInorder(bst);
            printf("\n");
        }
        else
        {
            printf("\nOperazione non consentita\n");
        }

        sleep(3);

        if ((err = sem_post(&sh->shared->sem[sh->n_thread])) != 0)
        {
            exit_with_err("sem_post", err);
        }
    }

    pthread_exit(NULL);
}

void padre(void *arg)
{
    int err;
    thread_data *sh = (thread_data *)arg;

    while (1)
    {
        if ((err = sem_wait(&sh->shared->sem[sh->n_thread])) != 0)
        {
            exit_with_err("sem_wait", err);
        }

        printf("\nInserisci una delle seguenti operazioni: \n");
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

        char *token = strtok(operazione, " ");

        char op[3][10];
        int i = 0;

        while (token != NULL)
        {
            strcpy(op[i++], token);
            token = strtok(NULL, " ");
        }

        sh->shared->numeroBST = atoi(op[1]);

        if (strcmp(op[0], "add") == 0)
        {
            sh->shared->operazione = INSERIMENTO;
            sh->shared->valore = atoi(op[2]);
        }
        else if (strcmp(op[0], "search") == 0)
        {
            sh->shared->operazione = RICERCA;
            sh->shared->valore = atoi(op[2]);
        }
        else if (strcmp(op[0], "print") == 0)
        {
            sh->shared->operazione = STAMPA;
        }
        else
        {
            sh->shared->operazione = -1;
        }

        if ((err = sem_post(&sh->shared->sem[sh->shared->numeroBST])) != 0)
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

    shared *sh = malloc(sizeof(shared));
    sh->sem = (sem_t *)malloc(argc * sizeof(sem_t));

    thread_data td[numeroBST + 1];

    init_shared(sh, numeroBST);

    // Creazione degli n-thread generici
    for (int i = 0; i < numeroBST; i++)
    {
        td[i].t_names = i;
        td[i].filename = argv[i + 1];
        td[i].n_thread = numeroBST;
        td[i].shared = sh;

        if ((err = pthread_create(&td[i].tid, NULL, (void *)bst_thread, (void *)&td[i])) != 0)
        {
            exit_with_err("pthread_create", err);
        }
    }

    // printf("Ho creato %d thread BST\n", numeroBST);

    // Creazione del thread padre
    td[numeroBST].t_names = numeroBST;
    td[numeroBST].shared = sh;
    td[numeroBST].n_thread = numeroBST;
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

    destroy_shared(sh, numeroBST);
    exit(EXIT_SUCCESS);
}