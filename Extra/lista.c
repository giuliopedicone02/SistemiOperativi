#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct _node_
{
    int val;
    struct _node_ *next;
} node;

typedef struct
{
    node *head;
    int size;
} lista;

int getSize(lista *l)
{
    return l->size;
}

void init_list(lista *l)
{
    l->head = NULL;
    l->size = 0;
}

void insert_list(lista *l, int val)
{
    node *aux;
    aux = malloc(sizeof(node));

    aux->val = val;

    aux->next = l->head;
    l->head = aux;

    l->size++;
}

node *listSearch(lista *l, int val)
{
    node *ptr = l->head;

    while (ptr)
    {
        if (ptr->val == val)
        {
            return ptr;
        }

        ptr = ptr->next;
    }

    return NULL;
}

void remove_list(lista *l, int val)
{
    node *ptr = l->head;
    node *prev = NULL;

    while (ptr)
    {
        if (ptr->val == val)
        {
            if (prev == NULL)
            {
                l->head = ptr->next;
            }
            else
            {
                prev->next = ptr->next;
            }

            free(ptr);
            l->size--;
            return;
        }

        prev = ptr;
        ptr = ptr->next;
    }
}

int getMin(lista *l)
{
    node *ptr = l->head;
    int min = INT_MAX;

    while (ptr)
    {
        if (ptr->val < min)
        {
            min = ptr->val;
        }

        ptr = ptr->next;
    }

    return min;
}

int getMax(lista *l)
{
    node *ptr = l->head;
    int max = INT_MIN;

    while (ptr)
    {
        if (ptr->val > max)
        {
            max = ptr->val;
        }

        ptr = ptr->next;
    }

    return max;
}

void sommaElementi(lista *l)
{
    while (getSize(l) != 1)
    {
        int somma = getMin(l) + getMax(l);

        remove_list(l, getMin(l));
        remove_list(l, getMax(l));
        insert_list(l, somma);
    }
}

void print_list(lista *l)
{
    node *ptr = l->head;

    printf("Lista: ");
    while (ptr->next)
    {
        printf("%d, ", ptr->val);
        ptr = ptr->next;
    }

    printf("%d\n", ptr->val);
}

void list_destroy(lista *l)
{
    node *ptr = l->head;
    node *tmp;

    while (ptr != NULL)
    {
        tmp = ptr;
        ptr = ptr->next;
        free(tmp);
    }

    free(l);
}

int main()
{
    lista *l = malloc(sizeof(lista));
    srand(time(NULL));

    init_list(l);

    for (int i = 0; i < 10; i++)
    {
        insert_list(l, rand() % 10 + 1);
    }

    print_list(l);
    printf("Dimensione della lista: %d\n\n", getSize(l));

    int cerca = 8;

    if (listSearch(l, cerca))
    {
        printf("Valore %d trovato\n", cerca);

        printf("Rimozione valore %d...\n", cerca);

        remove_list(l, cerca);

        print_list(l);
        printf("Dimensione della lista: %d\n\n", getSize(l));
    }
    else
    {
        printf("Valore %d non trovato\n\n", cerca);
    }

    sommaElementi(l);

    printf("Somma dei valori della lista...\n");
    print_list(l);

    list_destroy(l);
}
