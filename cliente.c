#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "pthread.h"
#include "signal.h"
#include "time.h"
#include "stdbool.h"
#include "sys/wait.h"

int serverPID;
int opc;
int eventType;

// Obtener el tipo de evento
char *getEventType(int type)
{
    switch (type)
    {
    case 1:
        return "LIMPIEZA";
    case 2:
        return "ACTUALIZACION";
    case 3:
        return "ENVIO";
    }
}

// Suscribirse a un evento de cierto tipo
void sub_event(int tipo)
{
    printf("Suscribirse a evento de tipo %s.\n", getEventType(tipo));
    // --- CODE HERE ---
    char comando[100];
    switch (tipo)
    {
    case 1:
        sprintf(comando, "kill -s ALRM %d", serverPID);
        system(comando);
        break;
    }
}

// Desuscribirse de un evento de cierto tipo
void unsub_event(int tipo)
{
    printf("Desuscribirse a evento de tipo %s.\n", getEventType(tipo));
    // --- CODE HERE ---
}

// Listar eventos a los que el cliente está suscrito
void list()
{
    printf("Listar eventos a los que estoy suscrito.\n");
    // --- CODE HERE ---
}

// Imprimir tipos de eventos disponibles
void ask()
{
    printf("Listar tipos de eventos.\n");
    // --- CODE HERE ---
}

// Imprimir algoritmos disponibles
void ask_algorithm()
{
    printf("Listar tipos de algoritmos.\n");
    // --- CODE HERE ---
}

int main()
{
    printf("PID del proceso: %d\n", getpid());

    printf("\nEscriba el PID del proceso padre: ");
    scanf("%d", &serverPID);

    while (1)
    {
        printf("MENÚ DE OPCIONES\n");
        printf("1) Suscribirse a un evento.\n");
        printf("2) Desuscribirse de un evento.\n");
        printf("3) Ver eventos a los que estoy suscrito.\n");
        printf("4) Listar eventos disponibles.\n");
        printf("5) Listar algoritmos disponibles.\n");
        scanf("%d", &opc);

        switch (opc)
        {
        case 1:
            printf("Ingrese el ID del evento al que se quiere suscribir: ");
            scanf("%d", &eventType);
            sub_event(eventType);
            break;
        case 2:
            printf("Ingrese el ID del evento del que se quiere desuscribir: ");
            scanf("%d", &eventType);
            unsub_event(eventType);
            break;
        case 3:
            list();
            break;
        case 4:
            ask();
            break;
        case 5:
            ask_algorithm();
            break;
        default:
            printf("Opción inválida.\n");
        }
    }
}
