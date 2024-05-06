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
    float response_ratio;
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
// Segundos actuales
int currentSeconds = 0;
int second_1 = 1;

// VARIABLES Y MÉTODOS PARA MLFQ
Evento q0[SIZE]; // High priority
Evento q1[SIZE];
Evento q2[SIZE]; // Low priority
int cabeza_q0 = 0, cola_q0 = 0, cabeza_q1 = 0, cola_q1 = 0, cabeza_q2 = 0, cola_q2 = 0;
int quantum_q0 = 5, quantum_q1 = 10;
int get_q0_length() { return cola_q0; }
int get_q1_length() { return cola_q1; }
int get_q2_length() { return cola_q2; }

// IMPRIMIR EVENTOS QUE HAN LLEGADO POR PARTE DEL CLIENTE
void print_queue(Evento q[], int head, int tail)
{
    if (head != tail)
    {
        printf("\nPROCESOS ACTUALES (T = %d s)\n", currentSeconds);
        for (int i = 0; i < tail; i++)
        {
            printf("ID %d (%d s)", q[i].id, q[i].remaining_time);
            if (i < tail - 1)
                printf("\t\t");
        }
        printf("\n\n");
    }
}

// GUARDAR EVENTO DEL CLIENTE (para ejecutarse después con un algoritmo)
void enqueue(Evento evento, Evento q[], int *tail)
{
    if (*tail == SIZE)
    {
        printf("La fila de procesos está llena.\n");
    }
    else
    {
        q[*tail] = evento;
        (*tail)++;
    }
}

// ELIMINAR EVENTO DEL CLIENTE
Evento dequeue(int index, Evento q[], int *head, int *tail)
{
    if (*head != *tail)
    {
        Evento evento = q[index];
        for (int i = index; i < *tail; i++)
        {
            q[i] = q[i + 1];
        }
        (*tail)--;
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

// Ejecución de thread
void *sleep_process(void *sleeping_time)
{
    int st = *(int *)sleeping_time;
    for (int i = 0; i < st; i++)
    {
        sleep(1);
        currentSeconds++;
    }
    pthread_exit(NULL);
}

// RECIBIR EVENTOS DEL CLIENTE
void funcion_usr1(int sig, siginfo_t *info, void *secret)
{
    int random_number = (rand() % 20) + 1; // Duración aleatoria
    Evento event = {globalID, random_number, random_number, currentSeconds, 0, 0, 0.0};
    printf("Señal recibida de %d. Creando proceso...\tID %d\t\tBT: %d s / AT: %d s\n", getpgid(info->si_pid), globalID, random_number, currentSeconds);
    enqueue(event, procesos, &cola);
    // Agregar a queue de MLFQ
    enqueue(event, q0, &cola_q0);
    globalID++;
}

// FIRST COME FIRST SERVE
void fcfs()
{
    if (!executing)
    {
        print_queue(procesos, cabeza, cola);
        Evento evento = dequeue(0, procesos, &cabeza, &cola);
        printf("FCFS: Ejecutar evento %d en %d segundos\n", evento.id, evento.burst_time);
        // Crear un thread con el evento y esperar a que éste termine
        executing = true;
        pthread_t id;
        pthread_create(&id, NULL, sleep_process, &evento.burst_time);
        pthread_join(id, NULL);
        executing = false;
        printf("Evento %d terminado\n", evento.id);
    }
}

// FIRST IN FIRST OUT
void fifo()
{
    if (!executing)
    {
        print_queue(procesos, cabeza, cola);
        Evento evento = dequeue(0, procesos, &cabeza, &cola);
        printf("FIFO: Ejecutar evento %d en %d segundos\n", evento.id, evento.burst_time);
        // Crear un thread con el evento y esperar a que éste termine
        executing = true;
        pthread_t id;
        pthread_create(&id, NULL, sleep_process, &evento.burst_time);
        pthread_join(id, NULL);
        executing = false;
        printf("Evento %d terminado\n", evento.id);
    }
}

// ROUND ROBIN
void round_robin()
{
    if (!executing)
    {
        print_queue(procesos, cabeza, cola);
        // Seleccionar primer evento
        Evento evento = dequeue(0, procesos, &cabeza, &cola);
        // Asignar quantum o tiempo restante del proceso
        int execute_time = (evento.remaining_time > quantum) ? quantum : evento.remaining_time;
        // Actualizar tiempo restante del proceso
        evento.remaining_time -= execute_time;
        // Ejecutar parte del proceso
        printf("RR: Ejecutar evento %d en %d segundos\n", evento.id, execute_time);
        // Crear un thread con el evento y esperar a que éste termine
        executing = true;
        pthread_t id;
        pthread_create(&id, NULL, sleep_process, &execute_time);
        pthread_join(id, NULL);
        executing = false;
        printf("Quantum de evento %d terminado, quedan %d segundos\n", evento.id, evento.remaining_time);
        // Volver a meter evento si aún no se termina
        if (evento.remaining_time > 0)
        {
            printf("Agregando evento %d nuevamente\n", evento.id);
            enqueue(evento, procesos, &cola);
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
        print_queue(procesos, cabeza, cola);
        // Encontrar trabajo más corto
        int minIndex = getShortestJob();
        Evento event = procesos[minIndex];
        printf("SJF: Ejecutar evento %d en %d segundos\n", event.id, event.burst_time);
        // Crear un thread con el evento y esperar a que éste termine
        executing = true;
        pthread_t id;
        pthread_create(&id, NULL, sleep_process, &event.burst_time);
        pthread_join(id, NULL);
        executing = false;
        dequeue(minIndex, procesos, &cabeza, &cola);
    }
}

// SHORTEST REMAINING TIME
void srt()
{
    if (!executing)
    {
        // Obtener trabajo más corto
        int minIndex = getShortestJob();
        Evento *evento = &procesos[minIndex]; // Apuntar a arreglo de procesos para poder actualizarlo posteriormente
        if (currentEventId != evento->id)
        {
            currentEventId = evento->id;
            print_queue(procesos, cabeza, cola);
            printf("SRT: Cambiando a proceso %d con tiempo de ejecución: %d s\n", evento->id, evento->remaining_time);
        }
        // Crear un thread con el evento y esperar a que éste termine
        executing = true;
        pthread_t id;
        int sleep_time = 1;
        pthread_create(&id, NULL, sleep_process, &sleep_time);
        pthread_join(id, NULL);
        executing = false;
        // Actualizar tiempo de evento
        evento->remaining_time--;
        // Eliminar evento si ya terminó
        if (evento->remaining_time <= 0)
        {
            printf("Evento %d terminado\n", evento->id);
            dequeue(minIndex, procesos, &cabeza, &cola);
        }
    }
}

// HIGHEST RESPONSE-RATIO NEXT
void hrrn()
{
    if (!executing)
    {
        print_queue(procesos, cabeza, cola);
        // Calcular response ratio de cada proceso
        int n = get_queue_length();
        float maxRR = 0;
        int maxIndex;
        printf("\n");
        for (int i = 0; i < n; i++)
        {
            Evento *evento = &procesos[i];
            evento->response_ratio = (evento->waiting_time + evento->burst_time) * 1.0 / evento->burst_time;
            printf("ID: %d\tBT: %d\tAT:%d\tWT: %d\tRR: %.3f\n", evento->id, evento->burst_time, evento->arrival_time, evento->waiting_time, evento->response_ratio);
            if (evento->response_ratio > maxRR)
            {
                maxIndex = i;
                maxRR = evento->response_ratio;
            }
        }
        Evento evento = procesos[maxIndex];
        printf("\nHRRN: Ejecutar evento %d en %d segundos (RR = %.3f)\n", evento.id, evento.burst_time, evento.response_ratio);
        // Crear un thread con el evento y esperar a que éste termine
        executing = true;
        for (int i = 0; i < evento.burst_time; i++)
        {
            pthread_t id;
            pthread_create(&id, NULL, sleep_process, &second_1);
            pthread_join(id, NULL);
        }
        executing = false;
        // Actualizar tiempos de espera de todos los procesos menos del actual
        int n2 = get_queue_length();
        for (int i = 0; i < n2; i++)
        {
            if (procesos[i].id != evento.id)
                procesos[i].waiting_time = currentSeconds - procesos[i].arrival_time;
        }
        printf("Evento %d terminado\n", evento.id);
        dequeue(maxIndex, procesos, &cabeza, &cola);
    }
}

// MULTILEVEL FEEDBACK QUEUES
void mlfq()
{
    if (!executing)
    {
        if (get_q0_length() != 0)
        {
            printf("\nMLFQ: Ejecutar evento de Q0 (quantum=5)\n");
            print_queue(q0, cabeza_q0, cola_q0);
            Evento e = dequeue(0, q0, &cabeza_q0, &cola_q0);
            // Asignar quantum o tiempo restante del proceso
            int execute_time = (e.remaining_time > quantum_q0) ? quantum_q0 : e.remaining_time;
            // Actualizar tiempo restante del proceso
            e.remaining_time -= execute_time;
            // Ejecutar parte del proceso
            printf("Ejecutar evento %d en %d segundos\n", e.id, execute_time);
            // Crear un thread con el evento y esperar a que éste termine
            executing = true;
            pthread_t id;
            pthread_create(&id, NULL, sleep_process, &execute_time);
            pthread_join(id, NULL);
            executing = false;
            printf("Quantum de evento %d terminado, quedan %d segundos\n", e.id, e.remaining_time);
            // Meter a siguiente queue si el evento aún no termina
            if (e.remaining_time > 0)
            {
                printf("Agregando evento %d a Q1\n", e.id);
                enqueue(e, q1, &cola_q1);
            }
            else
            {
                printf("Evento %d terminado\n", e.id);
            }
        }
        else if (get_q1_length() != 0)
        {
            printf("\nMLFQ: Ejecutar evento de Q1 (quantum=8)\n");
            print_queue(q1, cabeza_q1, cola_q1);
            Evento e = dequeue(0, q1, &cabeza_q1, &cola_q1);
            // Asignar quantum o tiempo restante del proceso
            int execute_time = (e.remaining_time > quantum_q1) ? quantum_q1 : e.remaining_time;
            // Actualizar tiempo restante del proceso
            e.remaining_time -= execute_time;
            // Ejecutar parte del proceso
            printf("Ejecutar evento %d en %d segundos\n", e.id, execute_time);
            // Crear un thread con el evento y esperar a que éste termine
            executing = true;
            pthread_t id;
            pthread_create(&id, NULL, sleep_process, &execute_time);
            pthread_join(id, NULL);
            executing = false;
            printf("Quantum de evento %d terminado, quedan %d segundos\n", e.id, e.remaining_time);
            // Meter a siguiente queue si el evento aún no termina
            if (e.remaining_time > 0)
            {
                printf("Agregando evento %d a Q2\n", e.id);
                enqueue(e, q2, &cola_q2);
            }
            else
            {
                printf("Evento %d terminado\n", e.id);
            }
        }
        else if (get_q2_length() != 0)
        {
            printf("\nMLFQ: Ejecutar evento de Q2 (FCFS)\n");
            print_queue(q2, cabeza_q2, cola_q2);
            Evento e = dequeue(0, q2, &cabeza_q2, &cola_q2);
            printf("Ejecutar evento %d en %d segundos\n", e.id, e.remaining_time);
            // Crear un thread con el evento y esperar a que éste termine
            executing = true;
            pthread_t id;
            pthread_create(&id, NULL, sleep_process, &e.remaining_time);
            pthread_join(id, NULL);
            executing = false;
            printf("Evento %d terminado\n", e.id);
        }
    }
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

    printf("Proceso con PID: %d\n", getpid());

    // Para pruebas
    Evento eventos[] = {
        {1, 5, 5, 0, 0, 0, 0.0},
        {2, 3, 3, 0, 0, 0, 0.0},
        {3, 4, 4, 0, 0, 0, 0.0},
        {4, 16, 16, 0, 0, 0, 0.0}};
    enqueue(eventos[0], procesos, &cola);
    enqueue(eventos[1], procesos, &cola);
    enqueue(eventos[2], procesos, &cola);
    enqueue(eventos[3], procesos, &cola);

    for (int i = 0; i < 4; i++)
    {
        Evento e = eventos[i];
        enqueue(e, q0, &cola_q0);
    }

    while (1)
    {
        /* if (!is_queue_empty()) {
            fcfs();
            // fifo();
            // round_robin();
            // sjf();
            // srt();
            // hrrn();
        } else {
            pthread_t id;
            pthread_create(&id, NULL, sleep_process, &second_1);
            pthread_join(id, NULL);
        } */
        if (get_q0_length() != 0 || get_q1_length() != 0 || get_q2_length() != 0)
        {
            mlfq();
        }
        else
        {
            pthread_t id;
            pthread_create(&id, NULL, sleep_process, &second_1);
            pthread_join(id, NULL);
        }
    }
}
