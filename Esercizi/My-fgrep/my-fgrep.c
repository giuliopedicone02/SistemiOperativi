#define _GNU_SOURCE
#include "../lib-misc.h"
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdbool.h>

#include <string.h>
#include <ctype.h>

#define FILTRO 0

typedef struct
{
    bool v;
    bool i;
    bool done;
    char parola[512];
    int turn;
    char riga[1024];
    pthread_mutex_t lock;
    pthread_cond_t *cond;
} sharedRF;

typedef struct
{
    char riga[1024];
    bool done;
    bool finish;
    pthread_mutex_t lock2;
    pthread_cond_t cond_filter;
    pthread_cond_t cond_main;
} sharedFM;

typedef struct
{
    pthread_t tid;
    char *file;
    int thread_name;
    sharedRF *RF;
    sharedFM *FM;
    int numerofile;
} thread_data;

bool applicaFiltro(char *parola, char *riga, bool i, bool v)
{
    if (i == true && v == true)
    {
        if ((strcasestr(riga, parola) != NULL) || (strstr(riga, parola) == NULL))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    if (i == false && v == true)
    {
        if (strstr(riga, parola) == NULL)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    if (i == true && v == false)
    {
        if (strcasestr(riga, parola) != NULL)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    if (i == false && v == false)
    {
        if (strstr(riga, parola) != NULL)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

void Lettore(void *arg)
{
    int err;
    char buffer[1024];
    thread_data *sh = (thread_data *)arg;

    FILE *fp = fopen(sh->file, "r");
    if (!fp)
    {
        printf("Errore nell'apertura del file: %s", sh->file);
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, 1024, fp))
    {
        if ((err = pthread_mutex_lock(&sh->RF->lock)) != 0)
        {
            exit_with_err("pthread_mutex_lock", err);
        }

        while (sh->RF->turn != sh->thread_name)
        {
            if ((err = pthread_cond_wait(&sh->RF->cond[sh->thread_name], &sh->RF->lock)) != 0)
            {
                exit_with_err("pthread_cond_wait", err);
            }
        }

        if (buffer[strlen(buffer) - 1] == '\n')
            buffer[strlen(buffer) - 1] = '\0';

        strcpy(sh->RF->riga, buffer);
        sh->RF->turn = 0; // Passo la linea letta al filtro

        if ((err = pthread_cond_signal(&sh->RF->cond[FILTRO])) != 0)
        {
            exit_with_err("pthread_cond_signal", err);
        }
        if ((err = pthread_mutex_unlock(&sh->RF->lock)) != 0)
        {
            exit_with_err("pthread_mutex_unlock", err);
        }
    }

    if ((err = pthread_mutex_lock(&sh->RF->lock)) != 0)
    {
        exit_with_err("pthread_mutex_lock", err);
    }
    while (sh->RF->turn != sh->thread_name)
    {
        if ((err = pthread_cond_wait(&sh->RF->cond[sh->thread_name], &sh->RF->lock)) != 0)
        {
            exit_with_err("pthread_cond_wait", err);
        }
    }

    sh->RF->done = true;
    sh->RF->turn = 0;

    if ((err = pthread_cond_signal(&sh->RF->cond[FILTRO])) != 0)
    {
        exit_with_err("pthread_cond_signal", err);
    }
    if ((err = pthread_mutex_unlock(&sh->RF->lock)) != 0)
    {
        exit_with_err("pthread_mutex_unlock", err);
    }

    // printf("Lettore %d termina\n", sh->thread_name);
    pthread_exit(NULL);
}

void Filtro(void *arg)
{
    int err;
    thread_data *sh = (thread_data *)arg;
    int threadAttuale = 1;

    while (1)
    {
        if ((err = pthread_mutex_lock(&sh->RF->lock)) != 0)
        {
            exit_with_err("pthread_mutex_lock", err);
        }

        while (FILTRO != sh->RF->turn)
        {
            if ((err = pthread_cond_wait(&sh->RF->cond[FILTRO], &sh->RF->lock)) != 0)
            {
                exit_with_err("pthread_cond_wait", err);
            }
        }

        if (sh->RF->turn > sh->numerofile)
        {
            break;
        }

        if (sh->RF->done)
        {
            threadAttuale++;
            sh->RF->done = false;
        }

        if (threadAttuale > sh->numerofile)
        {
            sh->FM->finish = true;
            sh->FM->done = 1;

            if ((err = pthread_cond_signal(&sh->FM->cond_main)) != 0)
            {
                exit_with_err("pthread_cond_signal", err);
            }

            if ((err = pthread_mutex_unlock(&sh->RF->lock)) != 0)
            {
                exit_with_err("pthread_mutex_unlock", err);
            }

            break;
        }

        if (applicaFiltro(sh->RF->parola, sh->RF->riga, sh->RF->i, sh->RF->v) == 1)
        {
            if ((err = pthread_mutex_lock(&sh->FM->lock2)) != 0)
            {
                exit_with_err("pthread_mutex_lock", err);
            }

            while (sh->FM->done != 0)
            {
                if ((err = pthread_cond_wait(&sh->FM->cond_filter, &sh->FM->lock2)) != 0)
                {
                    exit_with_err("pthread_cond_wait", err);
                }
            }

            strcpy(sh->FM->riga, sh->RF->riga);
            sh->FM->done = 1;
            // printf("%s\n", sh->RF->riga);

            if ((err = pthread_cond_signal(&sh->FM->cond_main)) != 0)
            {
                exit_with_err("pthread_cond_signal", err);
            }

            if ((err = pthread_mutex_unlock(&sh->FM->lock2)) != 0)
            {
                exit_with_err("pthread_mutex_unlock", err);
            }
        }

        sh->RF->turn = threadAttuale;

        if ((err = pthread_cond_signal(&sh->RF->cond[sh->RF->turn])) != 0)
        {
            exit_with_err("pthread_cond_signal", err);
        }
        if ((err = pthread_mutex_unlock(&sh->RF->lock)) != 0)
        {
            exit_with_err("pthread_mutex_unlock", err);
        }
    }

    // printf("Filtro termina\n");
    pthread_exit(NULL);
}

void Stampa(void *arg)
{
    int err;
    thread_data *sh = (thread_data *)arg;

    while (1)
    {
        if ((err = pthread_mutex_lock(&sh->FM->lock2)) != 0)
        {
            exit_with_err("pthread_mutex_lock", err);
        }

        while (sh->FM->done == 0)
        {
            if ((err = pthread_cond_wait(&sh->FM->cond_main, &sh->FM->lock2)) != 0)
            {
                exit_with_err("pthread_cond_wait", err);
            }
        }

        if (sh->FM->finish)
        {
            if ((err = pthread_mutex_unlock(&sh->FM->lock2)) != 0)
            {
                exit_with_err("pthread_mutex_unlock", err);
            }

            break;
        }

        printf("%s\n", sh->FM->riga);
        sh->FM->done = 0;

        if ((err = pthread_cond_signal(&sh->FM->cond_filter)) != 0)
        {
            exit_with_err("pthread_cond_signal", err);
        }

        if ((err = pthread_mutex_unlock(&sh->FM->lock2)) != 0)
        {
            exit_with_err("pthread_mutex_unlock", err);
        }
    }

    // printf("Stampa termina\n");
    pthread_exit(NULL);
}

void init_RF(sharedRF *sh, int numerofile)
{
    int err;
    sh->parola[0] = '\0';
    sh->i = false;
    sh->v = false;
    sh->done = false;
    sh->turn = 1;
    sh->riga[0] = '\0';

    if ((err = pthread_mutex_init(&sh->lock, NULL)) != 0)
    {
        exit_with_err("pthread_mutex_init", err);
    }

    for (int i = 0; i < numerofile + 1; i++)
    {
        if ((err = pthread_cond_init(&sh->cond[i], NULL)) != 0)
        {
            exit_with_err("pthread_cond_init", err);
        }
    }
}

void init_FM(sharedFM *sh)
{
    int err;
    sh->done = false;
    sh->finish = false;
    sh->riga[0] = '\0';
    if ((err = pthread_mutex_init(&sh->lock2, NULL)) != 0)
    {
        exit_with_err("pthread_mutex_init", err);
    }

    if ((err = pthread_cond_init(&sh->cond_filter, NULL)) != 0)
    {
        exit_with_err("pthread_cond_init", err);
    }

    if ((err = pthread_cond_init(&sh->cond_main, NULL)) != 0)
    {
        exit_with_err("pthread_cond_init", err);
    }
}

void destroy_shared_RF(sharedRF *sh, int numerofile)
{
    int err;

    for (int i = 0; i < numerofile + 1; i++)
    {
        if ((err = pthread_cond_destroy(&sh->cond[i])) != 0)
        {
            exit_with_err("pthread_cond_destroy", err);
        }
    }

    if ((err = pthread_mutex_destroy(&sh->lock)) != 0)
    {
        exit_with_err("pthread_mutex_destroy", err);
    }

    free(sh);
}

void destroy_shared_FM(sharedFM *sh)
{
    int err;

    if ((err = pthread_cond_destroy(&sh->cond_filter)) != 0)
    {
        exit_with_err("pthread_cond_destroy", err);
    }

    if ((err = pthread_cond_destroy(&sh->cond_main)) != 0)
    {
        exit_with_err("pthread_cond_destroy", err);
    }

    if ((err = pthread_mutex_destroy(&sh->lock2)) != 0)
    {
        exit_with_err("pthread_mutex_destroy2", err);
    }

    free(sh);
}

int main(int argc, char **argv)
{
    int err;
    if (argc < 3)
    {
        printf("Usage %s [-v] [-i] [word] <file-1> [file-2] [file-3] [...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    bool v, i;
    int cont = 1;
    v = i = false;

    if ((strcmp(argv[1], "-v") == 0) || (strcmp(argv[2], "-v") == 0))
    {
        v = true;
        cont++;
    }
    if ((strcmp(argv[1], "-i") == 0) || (strcmp(argv[2], "-i") == 0))
    {
        i = true;
        cont++;
    }

    char word[512];
    strcpy(word, argv[cont]);

    int numerofile;
    numerofile = (argc - 1) - cont;

    sharedRF *srf = malloc(sizeof(sharedRF));

    srf->cond = malloc(sizeof(pthread_cond_t) * (numerofile + 1));
    sharedFM *sfm = malloc(sizeof(sharedFM));

    thread_data td[numerofile + 2];
    init_RF(srf, numerofile);
    init_FM(sfm);

    // TH filtro
    td[0].thread_name = 0;
    td[0].numerofile = numerofile;
    td[0].RF = srf;
    td[0].FM = sfm;
    td[0].RF->i = i;
    td[0].RF->v = v;
    strcpy(td[0].RF->parola, word);
    if ((err = pthread_create(&td[0].tid, NULL, (void *)Filtro, (void *)&td[0])) != 0)
    {
        exit_with_err("pthread_create", err);
    }

    // TH reader
    for (int c = 1; c < numerofile + 1; c++)
    {
        td[c].thread_name = c;
        td[c].numerofile = numerofile;
        td[c].file = argv[cont + c];
        td[c].RF = srf;
        strcpy(td[c].RF->parola, word);

        if ((err = pthread_create(&td[c].tid, NULL, (void *)Lettore, (void *)&td[c])) != 0)
        {
            exit_with_err("pthread_create", err);
        }
    }

    // TH MAIN
    td[numerofile + 1].thread_name = numerofile + 1;
    td[numerofile + 1].numerofile = numerofile;
    td[numerofile + 1].FM = sfm;

    if ((err = pthread_create(&td[numerofile + 1].tid, NULL, (void *)Stampa, (void *)&td[numerofile + 1])) != 0)
    {
        exit_with_err("pthread_create", err);
    }

    for (int i = 0; i < numerofile + 2; i++)
    {
        if ((err = pthread_join(td[i].tid, NULL)) != 0)
        {
            exit_with_err("pthread_join", err);
        }
    }

    destroy_shared_RF(srf, numerofile);

    destroy_shared_FM(sfm);

    exit(EXIT_SUCCESS);
}
