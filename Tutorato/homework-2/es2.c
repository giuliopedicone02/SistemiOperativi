/**
 * Implementare il comando mv supportando i seguenti casi:
 *
 * spostamento sullo stesso filesystem: individuato tale caso, il file deve essere spostato utilizzando gli hard link
 * spostamento cross-filesystem: individuato tale caso, il file deve essere spostato utilizzando la strategia "copia & cancella";
 * spostamento di un link simbolico: individuato tale caso, il link simbolico deve essere ricreato a destinazione con lo stesso contenuto
 * (ovvero il percorso che denota l'oggetto referenziato); notate come tale spostamento potrebbe rendere il nuovo link simbolico non effettivamente valido.
 *
 * La sintassi da supportare è la seguente:
 *
 * $ homework-2 <pathname sorgente> <pathname destinazione>
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char buffer[50] = {0};

    if (argc < 3)
    {
        printf("Use: %s <pathname sorgente> <pathname destinazione>\n", argv[0]);
        return -1;
    }

    strcat(buffer, "mv ");
    strcat(buffer, argv[1]);
    strcat(buffer, " ");
    strcat(buffer, argv[2]);

    if (system(buffer) == 0)
    {
        printf("File spostato con successo\n");
    }
    else
    {
        perror("Spostamento non riuscito\n");
    }

    return 0;
}
