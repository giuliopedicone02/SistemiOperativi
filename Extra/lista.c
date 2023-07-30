#include <stdio.h>
#include <stdlib.h>

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
    while (ptr)
    {
        printf("%d ", ptr->val);
        ptr = ptr->next;
    }
    printf("\n");
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

    init_list(l);

    insert_list(l, 10);
    insert_list(l, 12);
    insert_list(l, 1);
    insert_list(l, 7);
    insert_list(l, 3);
    insert_list(l, 8);

    print_list(l);
    printf("Dimensione della lista: %d\n", getSize(l));

    int cerca = 8;

    if (listSearch(l, cerca))
    {
        printf("Valore %d trovato\n", cerca);
    }
    else
    {
        printf("Valore %d non trovato\n", cerca);
    }

    printf("Rimozione valore 12...\n");

    remove_list(l, 12);

    print_list(l);
    printf("Dimensione della lista: %d\n", getSize(l));

    sommaElementi(l);

    printf("Somma dei valori della lista...\n");
    print_list(l);

    list_destroy(l);
}
