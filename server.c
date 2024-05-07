#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "pthread.h"
#include "signal.h"
#include "time.h"
#include "stdbool.h"
#include "sys/wait.h"

#define SIZE 10000

// ----- EVENTOS ------

// Tipos de evento
enum TipoEvento
{
    LIMPIEZA = 1,
    ACTUALIZACION = 2,
    ENVIO = 3
};

// Estructura de un evento
typedef struct estructura
{
    int id;
    int burst_time;
    int remaining_time;
    int arrival_time;
    int turn_around_time;
    int waiting_time;
    float response_ratio;
    enum TipoEvento type;
} Evento;

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

// ----- VARIABLES GLOBALES -----
// Arreglo de eventos/peticiones que llegan por parte del cliente
Evento procesos[SIZE];
int cola = 0;
// Identificador de procesos
int globalID = 0;
// Ejecución de un proceso/evento
bool executing = false;
int status = 0;
int currentEventId = -1;
// Quantum para Round Robin
int quantum = 5;
// Segundos actuales
int currentSeconds = 0;
int second_1 = 1;
// Lista de clientes interesados
int interesadosL[SIZE], interesadosA[SIZE], interesadosE[SIZE];
// Algoritmo actual
int currentAlgorithm = 1;

// ----- VARIABLES Y MÉTODOS PARA MLFQ -----
Evento q0[SIZE]; // High priority
Evento q1[SIZE];
Evento q2[SIZE]; // Low priority
int cola_q0 = 0, cola_q1 = 0, cola_q2 = 0;
int quantum_q0 = 5, quantum_q1 = 10;
int get_q0_length() { return cola_q0; }
int get_q1_length() { return cola_q1; }
int get_q2_length() { return cola_q2; }

// ----- MANEJO DE EVENTOS -----

// Imprimir eventos
void print_queue(Evento q[], int tail)
{
    if (tail != 0)
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

// Guardar evento (para ejecutarse después con un algoritmo)
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

// Eliminar evento de una posición
Evento dequeue(int index, Evento q[], int *tail)
{
    if (*tail != 0)
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

// Cantidad de eventos
int get_queue_length()
{
    return cola;
}

// Muestra si la fila está vacía
bool is_queue_empty()
{
    if (cola == 0)
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

// ----- MANEJO DE THREADS -----

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

// ----- COMANDOS -----

// Salir del programa (exit)
void funcion_int_exit()
{
    printf("\nTerminando servidor.\n");
    printf("Enviando señal de terminación a todos los clientes.\n");
    // --- CODE HERE ---
    exit(0);
}

// Añadir un cliente a la lista de interesados
void add_event(pid_t idCliente, int type)
{
    printf("Añadir cliente con ID %d a lista de interesados %s\n", getpgid(idCliente), getEventType(type));
    // --- CODE HERE ---
}

// Eliminar un cliente de la lista de interesados
void remove_event(pid_t idCliente, int type)
{
    printf("Eliminar cliente con ID %d de lista de interesados %s\n", getpgid(idCliente), getEventType(type));
    // --- CODE HERE ---
}

// Añadir un evento a la fila
void trigger_event(int type)
{
    int random_number = (rand() % 20) + 1; // Duración aleatoria
    Evento event = {globalID, random_number, random_number, currentSeconds, 0, 0, 0.0, type};
    printf("Creando evento...\tID %d\t\tBT: %d s / AT: %d s / Tipo: %s\n\n", globalID, random_number, currentSeconds, getEventType(type));
    enqueue(event, procesos, &cola);
    enqueue(event, q0, &cola_q0); // Agregar a queue de MLFQ
    globalID++;
}

// Lista clientes suscritos a un evento
void list_event(int type)
{
    printf("Imprimir clientes de evento %s\n", getEventType(type));
    // --- CODE HERE ---
}

// Listar algoritmos disponibles
void list_algorithms()
{
    printf("Imprimir algoritmos disponibles.\n");
    // --- CODE HERE ---
}

// Manejo de cambio de algoritmo
void change_algorithm()
{
    int opc;
    printf("1) First Come First Server (FCFS)\n");
    printf("2) First In First Out (FIFO)\n");
    printf("3) Round Robin (RR)\n");
    printf("4) Shortest Job First (SJF)\n");
    printf("5) Shortest Remaining Time (SRT)\n");
    printf("6) Highest Response-Ratio Next (HRRN)\n");
    printf("7) Multilevel Feedback Queues (MLFQ)\n");
    scanf("%d", &opc);
    if (opc < 1 || opc > 7)
    {
        printf("Opción inválida.\n");
    }
    else
    {
        printf("\nCambiando algoritmo en próxima iteración.\n\n");
        currentAlgorithm = opc;
    }
}

// SIGUSR1: Cambio de algoritmo
void funcion_usr1(pid_t idCliente)
{
    printf("\nCliente con ID %d solicitó un cambio de algoritmo.\n", getpgid(idCliente));
    printf("Seleccione el tipo de algoritmo a cambiar.\n");
    change_algorithm();
}

// Manejador de señal SIGTSTP (trigger event_name, list event_name, list algorithm_name)
void funcion_tstp(int sig)
{
    int opc;
    int event_name;
    printf("\nSeleccione una opción\n");
    printf("1) trigger event_name\n");
    printf("2) list event_name\n");
    printf("3) list algorithm_name\n");
    scanf("%d", &opc);

    switch (opc)
    {
    case 1:
        printf("\nSeleccione el tipo de evento.\n");
        printf("1) LIMPIEZA\n");
        printf("2) ACTUALIZACION\n");
        printf("3) ENVIO\n");
        scanf("%d", &event_name);
        if (event_name < 1 || event_name > 3)
            printf("Opción inválida.\n");
        else
            trigger_event(event_name);
        break;
    case 2:
        printf("\nSeleccione el tipo de evento\n");
        printf("1) LIMPIEZA\n");
        printf("2) ACTUALIZACION\n");
        printf("3) ENVIO\n");
        scanf("%d", &event_name);
        if (event_name < 1 || event_name > 3)
            printf("Opción inválida.\n");
        else
            trigger_event(event_name);
        break;
    case 3:
        list_algorithms();
        break;
    default:
        printf("Opción inválida.\n");
        break;
    }
}

// Manejador de SIGACTION
void funcion_sigaction(int sig, siginfo_t *info, void *secret)
{
    printf("\nSeñal recibida de cliente con ID %d.\n", getpgid(info->si_pid));
    if (sig == SIGALRM)
    {
        add_event(info->si_pid, 1);
    }
    else if (sig == SIGTERM)
    {
        add_event(info->si_pid, 2);
    }
    else if (sig == SIGHUP)
    {
        add_event(info->si_pid, 3);
    }
    else if (sig == SIGBUS)
    {
        remove_event(info->si_pid, 1);
    }
    else if (sig == SIGSEGV)
    {
        remove_event(info->si_pid, 2);
    }
    else if (sig == SIGUSR2)
    {
        remove_event(info->si_pid, 3);
    }
    else if (sig == SIGUSR1)
    {
        funcion_usr1(info->si_pid);
    }
}

// Enviar señal SIGCONT a todos los clientes suscritos para notificar que un evento de un tipo comenzó
void send_start(int tipoEvento)
{
    switch (tipoEvento)
    {
    case 1:
        printf("Enviar señal SIGCONT a lista de clientes de que evento de LIMPIEZA comenzó.\n"); // Borrar esta línea
        // --- CODE HERE ---
        break;
    case 2:
        printf("Enviar señal SIGCONT a lista de clientes de que evento de ACTUALIZACION comenzó.\n"); // Borrar esta línea
        // --- CODE HERE ---
        break;
    case 3:
        printf("Enviar señal SIGCONT a lista de clientes de que evento de ENVIO comenzó.\n"); // Borrar esta línea
        // --- CODE HERE ---
        break;
    }
}

// Enviar señal SIGPIPE a todos los clientes suscritos para notificar que un evento de un tipo terminó
void send_end(int tipoEvento)
{
    switch (tipoEvento)
    {
    case 1:
        printf("Enviar señal SIGPIPE a lista de clientes de que evento de LIMPIEZA terminó.\n"); // Borrar esta línea
        // --- CODE HERE ---
        break;
    case 2:
        printf("Enviar señal SIGPIPE a lista de clientes de que evento de ACTUALIZACION terminó.\n"); // Borrar esta línea
        // --- CODE HERE ---
        break;
    case 3:
        printf("Enviar señal SIGPIPE a lista de clientes de que evento de ENVIO terminó.\n"); // Borrar esta línea
        // --- CODE HERE ---
        break;
    }
}

// ----- ALGORITMOS -----

// FIRST COME FIRST SERVE
void fcfs()
{
    if (!executing)
    {
        print_queue(procesos, cola);
        Evento evento = dequeue(0, procesos, &cola);
        printf("FCFS: Ejecutar evento %d en %d segundos\n", evento.id, evento.burst_time);
        send_start(evento.type);
        // Crear un thread con el evento y esperar a que éste termine
        executing = true;
        pthread_t id;
        pthread_create(&id, NULL, sleep_process, &evento.burst_time);
        pthread_join(id, NULL);
        executing = false;
        printf("Evento %d terminado\n", evento.id);
        send_end(evento.type);
    }
}

// FIRST IN FIRST OUT
void fifo()
{
    if (!executing)
    {
        print_queue(procesos, cola);
        Evento evento = dequeue(0, procesos, &cola);
        printf("FIFO: Ejecutar evento %d en %d segundos\n", evento.id, evento.burst_time);
        send_start(evento.type);
        // Crear un thread con el evento y esperar a que éste termine
        executing = true;
        pthread_t id;
        pthread_create(&id, NULL, sleep_process, &evento.burst_time);
        pthread_join(id, NULL);
        executing = false;
        printf("Evento %d terminado\n", evento.id);
        send_end(evento.type);
    }
}

// ROUND ROBIN
void round_robin()
{
    if (!executing)
    {
        print_queue(procesos, cola);
        // Seleccionar primer evento
        Evento evento = dequeue(0, procesos, &cola);
        // Asignar quantum o tiempo restante del proceso
        int execute_time = (evento.remaining_time > quantum) ? quantum : evento.remaining_time;
        // Actualizar tiempo restante del proceso
        evento.remaining_time -= execute_time;
        // Ejecutar parte del proceso
        printf("RR: Ejecutar evento %d en %d segundos\n", evento.id, execute_time);
        send_start(evento.type);
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
            send_end(evento.type);
        }
    }
}

// SHORTEST JOB FIRST
void sjf()
{
    if (!executing)
    {
        print_queue(procesos, cola);
        // Encontrar trabajo más corto
        int minIndex = getShortestJob();
        Evento event = procesos[minIndex];
        printf("SJF: Ejecutar evento %d en %d segundos\n", event.id, event.burst_time);
        send_start(event.type);
        // Crear un thread con el evento y esperar a que éste termine
        executing = true;
        pthread_t id;
        pthread_create(&id, NULL, sleep_process, &event.burst_time);
        pthread_join(id, NULL);
        executing = false;
        dequeue(minIndex, procesos, &cola);
        printf("Evento %d terminado\n", event.id);
        send_end(event.type);
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
            print_queue(procesos, cola);
            printf("SRT: Cambiando a proceso %d con tiempo de ejecución: %d s\n", evento->id, evento->remaining_time);
            send_start(evento->type);
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
            send_end(evento->type);
            dequeue(minIndex, procesos, &cola);
        }
    }
}

// HIGHEST RESPONSE-RATIO NEXT
void hrrn()
{
    if (!executing)
    {
        print_queue(procesos, cola);
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
        send_start(evento.type);
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
        dequeue(maxIndex, procesos, &cola);
        send_end(evento.type);
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
            print_queue(q0, cola_q0);
            Evento e = dequeue(0, q0, &cola_q0);
            send_start(e.type);
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
                send_end(e.type);
                for (int i = 0; i < get_queue_length(); i++)
                    if (procesos[i].id == e.id)
                        dequeue(i, procesos, &cola);
            }
        }
        else if (get_q1_length() != 0)
        {
            printf("\nMLFQ: Ejecutar evento de Q1 (quantum=8)\n");
            print_queue(q1, cola_q1);
            Evento e = dequeue(0, q1, &cola_q1);
            send_start(e.type);
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
                send_end(e.type);
                for (int i = 0; i < get_queue_length(); i++)
                    if (procesos[i].id == e.id)
                        dequeue(i, procesos, &cola);
            }
        }
        else if (get_q2_length() != 0)
        {
            printf("\nMLFQ: Ejecutar evento de Q2 (FCFS)\n");
            print_queue(q2, cola_q2);
            Evento e = dequeue(0, q2, &cola_q2);
            send_start(e.type);
            printf("Ejecutar evento %d en %d segundos\n", e.id, e.remaining_time);
            // Crear un thread con el evento y esperar a que éste termine
            executing = true;
            pthread_t id;
            pthread_create(&id, NULL, sleep_process, &e.remaining_time);
            pthread_join(id, NULL);
            executing = false;
            printf("Evento %d terminado\n", e.id);
            send_end(e.type);
            for (int i = 0; i < get_queue_length(); i++)
                if (procesos[i].id == e.id)
                    dequeue(i, procesos, &cola);
        }
    }
}

// ----- PROGRAMA PRINCIPAL -----
int main()
{
    // Semilla para números aleatorios
    srand(time(NULL));

    // Manejador de señales (Se usa sigaction para conocer quién mandó la señal)
    struct sigaction s;
    s.sa_sigaction = funcion_sigaction;
    sigemptyset(&s.sa_mask);
    s.sa_flags = SA_SIGINFO;
    sigaction(SIGALRM, &s, NULL);
    sigaction(SIGTERM, &s, NULL);
    sigaction(SIGHUP, &s, NULL);
    sigaction(SIGBUS, &s, NULL);
    sigaction(SIGSEGV, &s, NULL);
    sigaction(SIGUSR2, &s, NULL);
    sigaction(SIGUSR1, &s, NULL);
    signal(SIGTSTP, funcion_tstp);
    signal(SIGINT, funcion_int_exit);

    printf("SERVIDOR. Proceso con PID: %d\n", getpid());

    printf("\nSeleccione el algoritmo inicial.\n");
    change_algorithm();

    // Eventos de prueba
    Evento eventos[] = {
        {100, 5, 5, 0, 0, 0, 0.0, LIMPIEZA},
        {200, 3, 3, 0, 0, 0, 0.0, LIMPIEZA},
        {300, 4, 4, 0, 0, 0, 0.0, ENVIO},
        {400, 16, 16, 0, 0, 0, 0.0, ACTUALIZACION}};
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
        if (!is_queue_empty())
        {
            switch (currentAlgorithm)
            {
            case 1:
                fcfs();
                break;
            case 2:
                fifo();
                break;
            case 3:
                round_robin();
                break;
            case 4:
                sjf();
                break;
            case 5:
                srt();
                break;
            case 6:
                hrrn();
                break;
            case 7:
                mlfq();
                break;
            }
        }
        else
        {
            pthread_t id;
            pthread_create(&id, NULL, sleep_process, &second_1);
            pthread_join(id, NULL);
        }
    }
}
