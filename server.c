#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "pthread.h"
#include "signal.h"
#include "time.h"
#include "stdbool.h"
#include "sys/wait.h"

#define SIZE 10000

// ESTRUCTURA DE EVENTOS
typedef struct estructura
{
    int id;
    int burst_time;
    int remaining_time;
    int arrival_time;
    int turn_around_time;
    int waiting_time;
} Evento;

// VARIABLES GLOBALES
// Arreglo de eventos/peticiones que llegan por parte del cliente
Evento procesos[SIZE];
int cabeza = 0, cola = 0;
// Identificador de procesos
int globalID = 0;
// Ejecución de un proceso/evento
bool executing = false;
pid_t wpid;
int status = 0;

// IMPRIMIR EVENTOS QUE HAN LLEGADO POR PARTE DEL CLIENTE
void print_queue()
{
    if (cabeza != cola)
    {
        printf("\nPROCESOS ACTUALES\n");
        for (int i = 0; i < cola; i++)
        {
            printf("ID %d (%d s)", procesos[i].id, procesos[i].burst_time);
            if (i < cola - 1)
                printf("\t\t");
        }
        printf("\n\n");
    }
}

// GUARDAR EVENTO DEL CLIENTE (para ejecutarse después con un algoritmo)
void enqueue(Evento evento)
{
    if (cola == SIZE)
    {
        printf("La fila de procesos está llena.\n");
    }
    else
    {
        procesos[cola] = evento;
        cola++;
    }
}

// ELIMINAR EVENTO DEL CLIENTE
Evento dequeue(int index)
{
    if (cabeza != cola)
    {
        Evento evento = procesos[index];
        for (int i = index; i < cola; i++)
        {
            procesos[i] = procesos[i + 1];
        }
        cola--;
        return evento;
    }
}

// CANTIDAD DE EVENTOS DEL CLIENTE
int get_queue_length()
{
    return cola;
}

// MUESTRA SI HAY EVENTOS DEL CLIENTE
bool is_queue_empty()
{
    if (cabeza == cola)
        return true;
    else
        return false;
}

// RECIBIR EVENTOS DEL CLIENTE
void funcion_usr1(int sig)
{
    printf("Señal recibida de %d. Creando proceso con ID %d\n", getpid(), globalID);
    int random_number = (rand() % 20) + 1; // Duración aleatoria
    Evento event = {globalID, random_number, random_number, 0, 0, 0};
    enqueue(event);
    globalID++;
}

// FIRST COME FIRST SERVE
void fcfs()
{
    if (!is_queue_empty())
    {
        if (!executing)
        {
            print_queue();
            Evento event = dequeue(0);
            printf("FCFS: Ejecutar evento %d en %d segundos\n", event.id, event.burst_time);
            pid_t pid = fork();
            executing = true;
            if (pid == 0)
            {
                sleep(event.burst_time);
                printf("Evento %d terminado\n", event.id);
                exit(0);
            }
        }
    }
    else
    {
        sleep(5);
    }
}

// FIRST IN FIRST OUT
void fifo()
{
    if (!is_queue_empty())
    {
        if (!executing)
        {
            print_queue();
            Evento event = dequeue(0);
            printf("FIFO: Ejecutar evento %d en %d segundos\n", event.id, event.burst_time);
            pid_t pid = fork();
            executing = true;
            if (pid == 0)
            {
                sleep(event.burst_time);
                printf("Evento %d terminado\n", event.id);
                exit(0);
            }
        }
    }
    else
    {
        sleep(5);
    }
}

// SHORTEST JOB FIRST
void sjf()
{
    if (!is_queue_empty())
    {
        if (!executing)
        {
            print_queue();
            // Encontrar trabajo más corto
            int n = get_queue_length();
            int minIndex = -1;
            int minTime = 100000;
            for (int i = 0; i < n; i++)
            {
                if (procesos[i].burst_time < minTime)
                {
                    minIndex = i;
                    minTime = procesos[i].burst_time;
                }
            }
            Evento event = procesos[minIndex];
            printf("SJF: Ejecutar evento %d en %d segundos\n", event.id, event.burst_time);
            pid_t pid = fork();
            executing = true;
            if (pid == 0)
            {
                sleep(event.burst_time);
                printf("Evento %d terminado\n", event.id);
                exit(0);
            }
            while ((wpid = wait(&status)) > 0)
                ; // Esperar ejecución de evento
            dequeue(minIndex);
        }
    }
}

// TERMINACIÓN DE EVENTO
void funcion_chld(int sig)
{
    executing = false;
}

// PROGRAMA PRINCIPAL
int main()
{
    srand(time(NULL));

    signal(SIGUSR1, funcion_usr1);
    signal(SIGCHLD, funcion_chld);

    printf("Proceso con PID: %d\n", getpid());

    // Para pruebas
    /* Evento eventos[] = {
        {1, 5, 5, 0, 0, 0},
        {2, 1, 1, 0, 0, 0},
        {3, 2, 2, 0, 0, 0},
        {4, 6, 6, 0, 0, 0}
    };
    enqueue(eventos[0]);
    enqueue(eventos[1]);
    enqueue(eventos[2]);
    enqueue(eventos[3]); */

    while (1)
    {
        // fcfs();
        // fifo();
        sjf();
    }
}
