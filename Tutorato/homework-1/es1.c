/**
 * Scrivere un programma in linguaggio C che permetta di copiare un numero arbitrario
 * di file regolari su una directory di destinazione preesistente.
 * Il programma dovra' accettare una sintassi del tipo:
 *
 * $ homework-1 file1.txt path/file2.txt "nome con spazi.pdf" directory-destinazione
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char buffer[50] = {0};

    if (argc < 3)
    {
        printf("Use: %s <file1.txt> <path/file2.txt> <\"nome con spazi.pdf\"> <directory-destinazione>\n", argv[0]);
        return -1;
    }

    strcat(buffer, "cp");

    for (int i = 1; i < argc; i++)
    {
        strcat(buffer, " ");
        strcat(buffer, argv[i]);
    }

    if (system(buffer) == 0)
    {
        printf("Copia avvenuta con successo\n");
    }
    else
    {
        perror("Copia non riuscita\n");
    }

    return 0;
}
