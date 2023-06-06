#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define exit_with_sys_err(s)
{
    perror((s));
    exit(EXIT_FAILURE);
}

void my_exit1(void) { printf("Primo handler di uscita attivato!\n"); }
void my_exit2(void) { printf("Secondo handler di uscita attivato!\n"); }

int main(int argc, char **argv)
{
    printf("Avvio del programma...\n");

    if (atexit(my_exit2) != 0)
    {
        perror("my_exit2");
        exit(EXIT_FAILURE);
    }

    if (atexit(my_exit1) != 0)
    {
        exit_with_sys_err("my_exit1");
    }

    printf("Aspetto un po'...\n");
    sleep(3);

    printf("Chiudo il processo... \n");

    exit(EXIT_SUCCESS);
}