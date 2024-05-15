# Process-scheduling

## 1. Instrucciones de compilación, implementación y ejecución del proyecto

### Para correr el servidor
1. gcc server.c -o server
2. ./server

### Para correr un cliente (en una terminal nueva)
1. Instalar la librería Tkinter: sudo dnf install python3-tkinter (Fedora)
2. python cliente.py

---

## 2. Video de funcionamiento

URL a YouTube: PENDIENTE

---

## 3. Explicación de proyecto

PENDIENTE: Explicar cómo se resolvió cada requisito de la aplicación.

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