# Sistemas Operativos - Proyecto Final

## 1. Instrucciones de compilación, implementación y ejecución del proyecto

### Para correr el servidor
1. <code>gcc server.c -o server</code>
2. <code>./server</code>

### Para correr un cliente (en una terminal nueva)
1. Instalar la librería Tkinter: <code>sudo dnf install python3-tkinter</code> (Fedora)
2. <code>python cliente.py</code>

---

## 2. Video de funcionamiento

URL a YouTube: PENDIENTE

---

## 3. Explicación de proyecto

De forma resumida, los requisitos de la aplicación de se cumplen de la siguiente forma.

### Aplicación servidor

La aplicación servidor se encarga de <b>recibir señales</b> y <b>manejar eventos</b> que recibe por parte del cliente. Los eventos pueden ser de tipo <b>LIMPIEZA</b>, <b>ACTUALIZACIÓN</b> o <b>ENVÍO</b>, simulando tareas que puede realizar un sistema operativo.

#### Características del servidor
* Programa construido en C.
* Los eventos se manejan a través de 7 algoritmos:
    * First Come First Serve (FCFS)
    * FIrst In, First Out (FIFO)
    * Round Robin
    * Shortest Job First (SJF)
    * Shortest Remaining Time (SRT)
    * Highest Response-Ratio Next (HRRN)
    * Multilevel Feedback Queues (MLFQ)
* Solamente se tiene una instancia del servidor desde la terminal.
* Para identificar el PID del cliente que envía una señal, se utiliza el manejador de señales <code>sigaction()</code>. Específicamente, la estructura la estructura <code>siginfo_t</code> (proporcionada por sigaction) tiene un campo llamado <code>si_pid</code>, el cual contiene el PID del emisor de la señal.
* La ejecución de los eventos se maneja a través de Threads.
* El servidor puede recibir señales del cliente en cualquir momento para realizar un cambio de algoritmo.
* La fila de procesos listos se maneja a través de un arreglo.


#### Comandos del servidor
1. <code>exit</code>: Al recibir una señal SIGINT (Ctrl+C), termina el programa y envía una señal SIGINT a todos los clientes interesados.
2. <code>add event_name</code>: Añade a un cliente, por medio de su PID, a un arreglo de interesados de cierto evento (<code>interesadosL</code>, <code>interesadosA</code>, <code>interesadosE</code>).
3. <code>remove event_name</code>: Elimina a un cliente, por medio de su PID, de un arreglo de interesados de cierto evento (<code>interesadosL</code>, <code>interesadosA</code>, <code>interesadosE</code>).
4. <code>trigger event_name</code>: Crea un evento de cierto tipo (LIMPIEZA, ACTUALIZACIÓN, ENVÍO) y lo añade a la fila de listos. El evento se crea con los siguientes atributos:
    * <code>id</code>: Identificador del evento
    * <code>burst_time</code>: Tiempo de ráfaga.
    * <code>remaining_time</code>: Tiempo restante, el cual se va actualizando con la ejecución de los algoritmos.
    * <code>arrival_time</code>: Segundo (desde que inició el programa) en el que el evento llegó.
    * <code>turn_around_time</code>: Tiempo de retorno.
    * <code>waiting_time</code>: Tiempo de espera, el cual se va actualizando con la ejecución de los algoritmos.
    * <code>response_ratio</code>: Ratio de respuesta, útil para el algoritmo HRRN.
    * <code>type</code>: Tipo de evento (LIMPIEZA, ACTUALIZACIÓN, ENVÍO).
5. <code>list event_name</code>: Imprime el arreglo de clientes interesados en cierto tipo de evento (<code>interesadosL</code>, <code>interesadosA</code>, <code>interesadosE</code>).
6. <code>list algorithm_name</code>: Imprime los algoritmos disponibles.
7. <code>send_start</code>: Envía una señal a los clientes interesados de que un evento de cierto tipo ha comenzado su ejecución.
8. <code>send_end</code>: Envía una señal a los clientes interesados de que un evento de cierto tipo ha finalizado su ejecución.

### Aplicación cliente

La aplicación cliente se encarga de <b>enviar señales y eventos al servidor</b>, así como de <b>cambiar de algoritmo de planificación</b>.

#### Características del cliente
* Programa construido en Python, con la interfaz gráfica de Tkinter.
* Se pueden correr varios clientes al mismo tiempo, usando una terminal para cada uno.
* La interfaz gráfica del cliente tiene un espacio para escribir el PID del servidor al cual se enviarán las señales.
* La interfaz gráfica del cliente tiene un espacio para visualizar notificaciones del servidor.
* Cuando la interfaz gráfica del cliente se cierra (sin intervención del servidor), se envía una señal al servidor para desuscribirse de todos los eventos.

#### Comandos del cliente
1. <code>sub event_name</code>: Envía una señal al servidor para suscribirse a un evento de cierto tipo (LIMPIEZA, ACTUALIZACIÓN, ENVÍO).
2. <code>unsub event_name</code>: Envía una señal al servidor para desuscribirse a un evento de cierto tipo (LIMPIEZA, ACTUALIZACIÓN, ENVÍO).
3. <code>list</code>: Imprime todos los eventos a los cuales el cliente se encuentra suscrito.
4. <code>send_event</code>: Envía una señal al servidor para crear un tipo de evento (LIMPIEZA, ACTUALIZACIÓN, ENVÍO).
5. <code>ask</code>: Este comando está implícito en la interfaz gráfica ya que, por cada tipo de evento, se muestra un botón.
6. <code>ask_algorithm</code>: Este comando también está implícito en la interfaz gráfica ya que, por cada algoritmo, se muestra un botón. A través de estos botones, el cliente envía una señal al servidor para hacer el cambio de algoritmo.

---

## 4. Manejo de señales

<table>
    <tr>
        <th>Signal</th>
        <th>Servidor</th>
        <th>Cliente</th>
    </tr>
    <tr>
        <td>SIGHUP</td>
        <td>Cuando <b>recibe</b> la señal, añade al cliente a la lista de interesados del evento ENVIO.</td>
        <td><b>Envía</b> una señal al servidor para suscribirse al evento ENVIO.</td>
    </tr>
    <tr>
        <td>SIGINT</td>
        <td><b>Envía</b> una señal a todos los clientes para terminar el programa. Termina además el servidor.</td>
        <td>Termina el cliente.</td>
    </tr>
    <tr>
        <td>SIGQUIT</td>
        <td>Cuando <b>recibe</b> la señal, cambia el algoritmo a FIFO.</td>
        <td><b>Envía</b> una señal al servidor para cambiar el algoritmo a FIFO.</td>
    </tr>
    <tr>
        <td>SIGILL</td>
        <td>Cuando <b>recibe</b> la señal, cambia el algoritmo a Round Robin.</td>
        <td><b>Envía</b> una señal al servidor para cambiar el algoritmo a Round Robin.</td>
    </tr>
    <tr>
        <td>SIGTRAP</td>
        <td>Cuando <b>recibe</b> la señal, cambia el algoritmo a SJF.</td>
        <td><b>Envía</b> una señal al servidor para cambiar el algoritmo a SJF.</td>
    </tr>
    <tr>
        <td>SIGABRT</td>
        <td>Cuando <b>recibe</b> la señal, cambia el algoritmo a SRT.</td>
        <td><b>Envía</b> una señal al servidor para cambiar el algoritmo a SRT.</td>
    </tr>
    <tr>
        <td>SIGBUS</td>
        <td>Cuando <b>recibe</b> la señal, elimina al cliente de la lista de interesados del evento LIMPIEZA.</td>
        <td><b>Envía</b> una señal al servidor para desuscribirse del evento LIMPIEZA.</td>
    </tr>
    <tr>
        <td>SIGFPE</td>
        <td>Cuando <b>recibe</b> la señal, cambia el algoritmo a HRRN.</td>
        <td><b>Envía</b> una señal al servidor para cambiar el algoritmo a HRRN.</td>
    </tr>
    <tr>
        <td>SIGUSR1</td>
        <td>Cuando <b>recibe</b> la señal, cambia el algoritmo a FCFS.</td>
        <td><b>Envía</b> una señal al servidor para cambiar el algoritmo a FCFS.</td>
    </tr>
    <tr>
        <td>SIGSEGV</td>
        <td>Cuando <b>recibe</b> la señal, elimina al cliente de la lista de interesados del evento ACTUALIZACION.</td>
        <td><b>Envía</b> una señal al servidor para desuscribirse del evento ACTUALIZACION.</td>
    </tr>
    <tr>
        <td>SIGUSR2</td>
        <td>Cuando <b>recibe</b> la señal, elimina al cliente de la lista de interesados del evento ENVIO.</td>
        <td><b>Envía</b> una señal al servidor para desuscribirse del evento ENVIO.</td>
    </tr>
    <tr>
        <td>SIGPIPE</td>
        <td><b>Envía</b> una señal a los clientes de que un evento de tipo LIMPIEZA ha finalizado su ejecución.</td>
        <td>Cuando <b>recibe</b> la señal, el cliente es notificado de que un evento de tipo LIMPIEZA ha finalizado su ejecución.</td>
    </tr>
    <tr>
        <td>SIGALRM</td>
        <td>Cuando <b>recibe</b> la señal, añade al cliente a la lista de interesados del evento LIMPIEZA.</td>
        <td><b>Envía</b> una señal al servidor para suscribirse al evento LIMPIEZA.</td>
    </tr>
    <tr>
        <td>SIGTERM</td>
        <td>Cuando <b>recibe</b> la señal, añade al cliente a la lista de interesados del evento ACTUALIZACION.</td>
        <td><b>Envía</b> una señal al servidor para suscribirse al evento ACTUALIZACION.</td>
    </tr>
    <tr>
        <td>SIGSTKFLT</td>
        <td>Cuando <b>recibe</b> la señal, cambia el algoritmo a MLFQ.</td>
        <td><b>Envía</b> una señal al servidor para cambiar el algoritmo a MLFQ.</td>
    </tr>
    <tr>
        <td>SIGCONT</td>
        <td><b>Envía</b> una señal a los clientes de que un evento de tipo LIMPIEZA ha comenzado su ejecución.</td>
        <td>Cuando <b>recibe</b> la señal, el cliente es notificado de que un evento de tipo LIMPIEZA ha comenzado su ejecución.</td>
    </tr>
    <tr>
        <td>SIGTSTP</td>
        <td>Permite listar los clientes suscritos a un evento en particular, o bien, listar los algoritmos disponibles.</td>
        <td>N/A</td>
    </tr>
    <tr>
        <td>SIGTTIN</td>
        <td>Cuando <b>recibe</b> la señal, crea un proceso de tipo LIMPIEZA.</td>
        <td><b>Envía</b> una señal al servidor para crear un proceso de tipo LIMPIEZA.</td>
    </tr>
    <tr>
        <td>SIGTTOU</td>
        <td>Cuando <b>recibe</b> la señal, crea un proceso de tipo ACTUALIZACION.</td>
        <td><b>Envía</b> una señal al servidor para crear un proceso de tipo ACTUALIZACION.</td>
    </tr>
    <tr>
        <td>SIGURG</td>
        <td>Cuando <b>recibe</b> la señal, crea un proceso de tipo ENVIO.</td>
        <td><b>Envía</b> una señal al servidor para crear un proceso de tipo ENVIO.</td>
    </tr>
    <tr>
        <td>SIGXCPU</td>
        <td><b>Envía</b> una señal a los clientes de que un evento de tipo ACTUALIZACION ha comenzado su ejecución.</td>
        <td>Cuando <b>recibe</b> la señal, el cliente es notificado de que un evento de tipo ACTUALIZACION ha comenzado su ejecución.</td>
    </tr>
    <tr>
        <td>SIGXFSZ</td>
        <td><b>Envía</b> una señal a los clientes de que un evento de tipo ENVIO ha comenzado su ejecución.</td>
        <td>Cuando <b>recibe</b> la señal, el cliente es notificado de que un evento de tipo ENVIO ha comenzado su ejecución.</td>
    </tr>
    <tr>
        <td>SIGVTALRM</td>
        <td><b>Envía</b> una señal a los clientes de que un evento de tipo ACTUALIZACION ha finalizado su ejecución.</td>
        <td>Cuando <b>recibe</b> la señal, el cliente es notificado de que un evento de tipo ACTUALIZACION ha finalizado su ejecución.</td>
    </tr>
    <tr>
        <td>SIGPROF</td>
        <td><b>Envía</b> una señal a los clientes de que un evento de tipo ENVIO ha finalizado su ejecución.</td>
        <td>Cuando <b>recibe</b> la señal, el cliente es notificado de que un evento de tipo ENVIO ha finalizado su ejecución.</td>
    </tr>
</table>