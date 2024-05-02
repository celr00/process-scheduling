#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "pthread.h"
#include "signal.h"
#include "time.h"
#include "stdbool.h"
#include "sys/wait.h"

int serverPID;

int main()
{
    printf("PID del proceso: %d\n", getpid());

    printf("\nEscriba el PID del proceso padre: ");
    scanf("%d", &serverPID);

    printf("Enviando señal...\n");
    char comando[100];
    sprintf(comando, "kill -s USR1 %d", serverPID);
    system(comando);
}