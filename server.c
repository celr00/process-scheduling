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
int currentEventId = -1;
// Quantum para Round Robin
int quantum = 5;

// IMPRIMIR EVENTOS QUE HAN LLEGADO POR PARTE DEL CLIENTE
void print_queue()
{
    if (cabeza != cola)
    {
        printf("\nPROCESOS ACTUALES\n");
        for (int i = 0; i < cola; i++)
        {
            printf("ID %d (%d s)", procesos[i].id, procesos[i].remaining_time);
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

// Regresar posición de trabajo con menor tiempo de ejecución
int getShortestJob()
{
    int n = get_queue_length();
    int minIndex = -1;
    int minTime = 100000;
    for (int i = 0; i < n; i++)
    {
        if (procesos[i].remaining_time < minTime)
        {
            minIndex = i;
            minTime = procesos[i].remaining_time;
        }
    }
    return minIndex;
}

// RECIBIR EVENTOS DEL CLIENTE
void funcion_usr1(int sig, siginfo_t *info, void *secret)
{
    int random_number = (rand() % 20) + 1; // Duración aleatoria
    Evento event = {globalID, random_number, random_number, 0, 0, 0};
    printf("Señal recibida de %d. Creando proceso con ID %d (Burst Time: %d s)\n", getpgid(info->si_pid), globalID, random_number);
    enqueue(event);
    globalID++;
}

// FIRST COME FIRST SERVE
void fcfs()
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

// FIRST IN FIRST OUT
void fifo()
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

// ROUND ROBIN
void round_robin()
{
    if (!executing)
    {
        print_queue();
        // Seleccionar primer evento
        Evento evento = dequeue(0);
        // Asignar quantum o tiempo restante del proceso
        int execute_time = (evento.remaining_time > quantum) ? quantum : evento.remaining_time;
        // Actualizar tiempo restante del proceso
        evento.remaining_time -= execute_time;
        // Ejecutar parte del proceso
        printf("RR: Ejecutar evento %d en %d segundos\n", evento.id, execute_time);
        pid_t pid = fork();
        executing = true;
        if (pid == 0)
        {
            sleep(execute_time);
            printf("Quantum de evento %d terminado, quedan %d segundos\n", evento.id, evento.remaining_time);
            exit(0);
        }
        while ((wpid = wait(&status)) > 0)
            ; // Esperar ejecución de evento
        // Volver a meter evento si aún no se termina
        if (evento.remaining_time > 0)
        {
            printf("Agregando evento %d nuevamente\n", evento.id);
            enqueue(evento);
        }
        else
        {
            printf("Evento %d terminado\n", evento.id);
        }
    }
}

// SHORTEST JOB FIRST
void sjf()
{
    if (!executing)
    {
        print_queue();
        // Encontrar trabajo más corto
        int minIndex = getShortestJob();
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

// SHORTEST REMAINING TIME
void srt()
{
    if (!executing)
    {
        // print_queue();
        // Obtener trabajo más corto
        int minIndex = getShortestJob();
        // printf("Mínimo índice actual %d\n", minIndex);
        Evento *evento = &procesos[minIndex]; // Apuntar a arreglo de procesos para poder actualizarlo posteriormente
        if (currentEventId != evento->id)
        {
            currentEventId = evento->id;
            print_queue();
            printf("SRT: Cambiando a proceso %d con tiempo de ejecución: %d\n", evento->id, evento->remaining_time);
        }
        pid_t pid = fork();
        executing = true;
        if (pid == 0)
        {
            sleep(1);
            exit(0);
        }
        while ((wpid = wait(&status)) > 0)
            ; // Esperar ejecución de evento
        // Actualizar tiempo de evento
        evento->remaining_time--;
        // Eliminar evento si ya terminó
        if (evento->remaining_time <= 0)
        {
            printf("Evento %d terminado\n", evento->id);
            dequeue(minIndex);
        }
    }
}

// HIGHEST RESPONSE-RATIO NEXT
void hrrn()
{
    if (!executing)
    {
        print_queue();
        // Calcular response ratio de cada proceso
        int n = get_queue_length();
        float maxRR = 0;
        int maxIndex;
        printf("\n");
        for (int i = 0; i < n; i++)
        {
            Evento *evento = &procesos[i];
            evento->response_ratio = (evento->waiting_time + evento->burst_time) * 1.0 / evento->burst_time;
            printf("ID: %d\tBT: %d\tWT: %d\tRR: %.3f\n", evento->id, evento->burst_time, evento->waiting_time, evento->response_ratio);
            if (evento->response_ratio > maxRR)
            {
                maxIndex = i;
                maxRR = evento->response_ratio;
            }
        }
        Evento evento = procesos[maxIndex];
        printf("\nHRRN: Ejecutar evento %d en %d segundos (RR = %.3f)\n", evento.id, evento.burst_time, evento.response_ratio);
        // Crear un thread con el proceso
        pthread_t id;
        pthread_create(&id, NULL, sleep_process, &evento.burst_time);
        int *ptr;
        // Esperar a que termine el thread
        pthread_join(id, (void **)&ptr);
        executing = false;
        // Actualizar tiempos de espera de todos los procesos menos del actual
        int n2 = get_queue_length();
        for (int i = 0; i < n2; i++)
        {
            if (procesos[i].id != evento.id)
            {
                procesos[i].waiting_time += evento.burst_time;
            }
        }
        dequeue(maxIndex);
    }
}

// MULTILEVEL FEEDBACK QUEUES
void mlfq()
{
    printf("Multilevel Feedback Queues\n");
}

// TERMINACIÓN DE EVENTO
void funcion_chld(int sig)
{
    executing = false;
}

// PROGRAMA PRINCIPAL
int main()
{
    // Semilla para números aleatorios
    srand(time(NULL));

    // Manejador de señales de SIGUSR1 (Se usa sigaction para conocer quién mandó la señal)
    struct sigaction s;
    s.sa_sigaction = funcion_usr1;
    sigemptyset(&s.sa_mask);
    s.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &s, NULL);

    // Manejador de señales de SIGCHLD (Notifica cuándo se termina un evento)
    signal(SIGCHLD, funcion_chld);

    printf("Proceso con PID: %d\n", getpid());

    // Para pruebas
    /* Evento eventos[] = {
        {1, 5, 5, 0, 0, 0},
        {2, 3, 3, 0, 0, 0},
        {3, 4, 4, 0, 0, 0},
        {4, 6, 6, 0, 0, 0}
    };
    enqueue(eventos[0]);
    enqueue(eventos[1]);
    enqueue(eventos[2]);
    enqueue(eventos[3]); */

    while (1)
    {
        if (!is_queue_empty())
        {
            // fcfs();
            // fifo();
            // sjf();
            // round_robin();
            srt();
        }
        else
        {
            sleep(5);
        }
    }
}
