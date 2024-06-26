import tkinter as tk
import os
import signal
import sys

### EVENTOS ###
events = []


### MANEJO DE SEÑALES ###

# Salir del programa
def funcion_int(sig, frame):
    toClose = events[:]
    for event in toClose:
        unsub_event(event)
    window.destroy()
    sys.exit(0)

# Notificar inicio de un tipo de evento
def event_start(sig, frame):
    eventType = "LIMPIEZA" if sig==signal.SIGCONT else "ACTUALIZACION" if sig==signal.SIGXCPU else "ENVIO" if sig==signal.SIGXFSZ else ""
    results_label.insert(tk.END, "Un evento de tipo {} ha comenzado su ejecución.\n".format(eventType))

# Notificar fin de un tipo de evento
def event_end(sig, frame):
    eventType = "LIMPIEZA" if sig==signal.SIGPIPE else "ACTUALIZACION" if sig==signal.SIGVTALRM else "ENVIO" if sig==signal.SIGPROF else ""
    results_label.insert(tk.END, "Un evento de tipo {} ha terminado su ejecución.\n".format(eventType))


### MANEJO DE COMANDOS ###

# Suscribirse a un evento
def sub_event(eventType):
    server_pid = server_pid_txt.get()
    if not server_pid:
        results_label.insert(tk.END, "ERR: Ingrese el PID del servidor primero.\n")
    else:
        print("Suscribir {}".format(eventType))
        # Enviar una señal de tipo SIGALRM / SIGTERM / SIGHUP al servidor, según el tipo de evento
        if eventType == "LIMPIEZA":
            os.system("kill -s ALRM {}".format(server_pid))
        elif eventType == "ACTUALIZACION":
            os.system("kill -s TERM {}".format(server_pid))
        elif eventType == "ENVIO":
            os.system("kill -s HUP {}".format(server_pid))

        if eventType not in events:
            events.append(eventType)
            results_label.insert(tk.END, f"Se ha suscrito al evento de {eventType}.\n")

# Desuscribirse de un evento
def unsub_event(eventType):
    server_pid = server_pid_txt.get()
    if not server_pid:
        results_label.insert(tk.END, "ERR: Ingrese el PID del servidor primero.\n")
    else:
        print("Desuscribir {}".format(eventType))
        # Enviar una señal de tipo SIGBUS / SIGSEGV / SIGUSR2 al servidor, según el tipo de evento
        if eventType == "LIMPIEZA":
            os.system("kill -s BUS {}".format(server_pid))
        elif eventType == "ACTUALIZACION":
            os.system("kill -s SEGV {}".format(server_pid))
        elif eventType == "ENVIO":
            os.system("kill -s USR2 {}".format(server_pid))

        if eventType in events:
            events.remove(eventType)
        results_label.insert(tk.END, f"Se ha desuscrito al evento de {eventType}.\n")

# Listar eventos a los que el cliente está suscrito
def list_events():
    print("Listar eventos a los que el cliente está suscrito")
    msg = ", ".join(events)
    results_label.insert(tk.END, f"Suscrito a: {msg}.\n")

# Enviar al servidor una solicitud de cambio de algoritmo
def change_algorithm(algorithm):
    server_pid = server_pid_txt.get()
    if not server_pid:
        results_label.insert(tk.END, "Ingrese el PID del servidor primero.\n")
    else:
        if algorithm==1:
            os.system("kill -s USR1 {}".format(server_pid))
        elif algorithm==2:
            os.system("kill -s QUIT {}".format(server_pid))
        elif algorithm==3:
            os.system("kill -s ILL {}".format(server_pid))
        elif algorithm==4:
            os.system("kill -s TRAP {}".format(server_pid))
        elif algorithm==5:
            os.system("kill -s ABRT {}".format(server_pid))
        elif algorithm==6:
            os.system("kill -s FPE {}".format(server_pid))
        elif algorithm==7:
            os.system("kill -s STKFLT {}".format(server_pid))
        results_label.insert(tk.END, f"Algoritmo cambiado.\n")

# Enviar al servidor una solicitud para crear un tipo de evento
def send_event(eventType):
    server_pid = server_pid_txt.get()
    if not server_pid:
        results_label.insert(tk.END, "Ingrese el PID del servidor primero.\n")
    else:
        if eventType == 1:
            os.system("kill -s TTIN {}".format(server_pid))
        if eventType == 2:
            os.system("kill -s TTOU {}".format(server_pid))
        if eventType == 3:
            os.system("kill -s URG {}".format(server_pid))
        eventTypeStr = "LIMPIEZA" if eventType==1 else "ACTUALIZACION" if eventType==2 else "ENVIO" if eventType==3 else ""
        results_label.insert(tk.END, f"Evento de tipo {eventTypeStr} enviado al servidor.\n")

# Cerrar ventana -> Enviar al servidor notificación de desuscripción
def on_closing():
    toClose = events[:]
    for event in toClose:
        unsub_event(event)
    window.destroy()


### INICIALIZACIÓN DE INTERFAZ GRÁFICA ###

window = tk.Tk()
window.title("Cliente con PID {}".format(os.getpid()))
window.option_add("*Button*Background", "#7FB3D5")

# Obtener las dimensiones de la pantalla
ancho_pantalla = window.winfo_screenwidth()
alto_pantalla = window.winfo_screenheight()

# Calcular las coordenadas para centrar la ventana
ancho_window = 800
alto_window = 750
posicion_x = (ancho_pantalla - ancho_window) // 2
posicion_y = (alto_pantalla - alto_window) // 2

# Establecer el tamaño y la posición de la window
window.geometry(f"{ancho_window}x{alto_window}+{posicion_x}+{posicion_y}")

# Establecer pesos de columnas y filas
window.columnconfigure(0, weight=1, uniform="a")


### SECCIÓN 1: PID DE SERVIDOR ###

s1 = tk.Frame(window, bd=2, pady=10, padx=10)
s1.grid(row=0, column=0, sticky="nsew")
s1.columnconfigure(0, weight=1, uniform="a")

tk.Label(s1, text="Escriba el PID del servidor...", font=("Verdana", 14), bg="#372FAF", fg="white").grid(row=0, column=0, sticky="nsew", padx=10)
server_pid_txt = tk.Entry(s1, font=("Verdana", 14))
server_pid_txt.grid(row=1, column=0, sticky="nsew", padx=10)


### SECCIÓN 2: SUSCRIBIR, DESUSCRIBIR Y VER EVENTOS ###

s2 = tk.Frame(window, bd=2, pady=10, padx=10)
s2.grid(row=1, column=0, sticky="nsew")
s2.columnconfigure((0,1,2), weight=1, uniform="a")

# Sucribirse a un evento
sub_frame = tk.Frame(s2, padx=10)
sub_frame.grid(row=0, column=0, sticky="nsew")
sub_frame.columnconfigure(0, weight=1, uniform="a")
tk.Label(sub_frame, text="sub event_name", font=("Verdana", 14), bg="#372FAF", fg="white").grid(row=0, column=0, sticky="nsew", pady=(0,5))
tk.Button(sub_frame, text='LIMPIEZA', font=("Verdana", 12), command=lambda:sub_event("LIMPIEZA")).grid(row=1, column=0, sticky="nsew", pady=5)
tk.Button(sub_frame, text='ACTUALIZACION', font=("Verdana", 12), command=lambda:sub_event("ACTUALIZACION")).grid(row=2, column=0, sticky="nsew", pady=5)
tk.Button(sub_frame, text='ENVIO', font=("Verdana", 12), command=lambda:sub_event("ENVIO")).grid(row=3, column=0, sticky="nsew", pady=5)

# Desuscribirse de un evento
unsub_frame = tk.Frame(s2, padx=10)
unsub_frame.grid(row=0, column=1, sticky="nsew")
unsub_frame.columnconfigure(0, weight=1, uniform="a")
tk.Label(unsub_frame, text="unsub event_name", font=("Verdana", 14), bg="#372FAF", fg="white").grid(row=0, column=0, sticky="nsew", pady=(0,5))
tk.Button(unsub_frame, text='LIMPIEZA', font=("Verdana", 12), command=lambda:unsub_event("LIMPIEZA")).grid(row=1, column=0, sticky="nsew", pady=5)
tk.Button(unsub_frame, text='ACTUALIZACION', font=("Verdana", 12), command=lambda:unsub_event("ACTUALIZACION")).grid(row=2, column=0, sticky="nsew", pady=5)
tk.Button(unsub_frame, text='ENVIO', font=("Verdana", 12), command=lambda:unsub_event("ENVIO")).grid(row=3, column=0, sticky="nsew", pady=5)

# Ver eventos a los que me encuentro suscrito
liste_frame = tk.Frame(s2, padx=10)
liste_frame.grid(row=0, column=2, sticky="nsew")
liste_frame.columnconfigure(0, weight=1, uniform="a")
tk.Label(liste_frame, text="list", font=("Verdana", 14), bg="#372FAF", fg="white").grid(row=0, column=0, sticky="nsew", pady=(0,5))
tk.Button(liste_frame, text='Mis suscripciones', font=("Verdana", 12), command=list_events).grid(row=1, column=0, sticky="nsew", pady=5)


### SECCIÓN 3: DESENCADENAR UN EVENTO EN EL SERVIDOR ###
s3 = tk.Frame(window, bd=2, pady=10, padx=10)
s3.grid(row=2, column=0, sticky="nsew")
s3.columnconfigure((0,1,2), weight=1, uniform="a")

tk.Label(s3, text="Enviar un evento al servidor", font=("Verdana", 14), bg="#372FAF", fg="white").grid(row=0, column=0, columnspan=3, sticky="nsew", pady=(0,5), padx=10)
tk.Button(s3, text='LIMPIEZA', font=("Verdana", 12), command=lambda:send_event(1)).grid(row=1, column=0, sticky="nsew", pady=5, padx=10)
tk.Button(s3, text='ACTUALIZACION', font=("Verdana", 12), command=lambda:send_event(2)).grid(row=1, column=1, sticky="nsew", pady=5, padx=10)
tk.Button(s3, text='ENVIO', font=("Verdana", 12), command=lambda:send_event(3)).grid(row=1, column=2, sticky="nsew", pady=5, padx=10)


### SECCIÓN 4: CAMBIO DE ALGORITMOS ###

s4 = tk.Frame(window, bd=2, pady=10, padx=10)
s4.grid(row=3, column=0, sticky="nsew")
s4.columnconfigure((0,1,2), weight=1, uniform="a")

tk.Label(s4, text="Cambio de algoritmo", font=("Verdana", 14), bg="#372FAF", fg="white").grid(row=0, column=0, columnspan=3, sticky="nsew", pady=(0,5), padx=10)
tk.Button(s4, text='First Come First Serve', font=("Verdana", 12), command=lambda:change_algorithm(1)).grid(row=1, column=0, sticky="nsew", pady=5, padx=10)
tk.Button(s4, text='First In First Out', font=("Verdana", 12), command=lambda:change_algorithm(2)).grid(row=1, column=1, sticky="nsew", pady=5, padx=10)
tk.Button(s4, text='Round Robin', font=("Verdana", 12), command=lambda:change_algorithm(3)).grid(row=1, column=2, sticky="nsew", pady=5, padx=10)
tk.Button(s4, text='Shortest Job First', font=("Verdana", 12), command=lambda:change_algorithm(4)).grid(row=2, column=0, sticky="nsew", pady=5, padx=10)
tk.Button(s4, text='Shortest Remaining Time', font=("Verdana", 12), command=lambda:change_algorithm(5)).grid(row=2, column=1, sticky="nsew", pady=5, padx=10)
tk.Button(s4, text='Highest Response-Ratio Next', font=("Verdana", 12), command=lambda:change_algorithm(6)).grid(row=2, column=2, sticky="nsew", pady=5, padx=10)
tk.Button(s4, text='Multilevel Feedback Queues', font=("Verdana", 12), command=lambda:change_algorithm(7)).grid(row=3, column=0, sticky="nsew", pady=5, padx=10)


### SECCIÓN 5: DESPLIEGUE DE RESULTADOS ###

s5 = tk.Frame(window, bd=2, pady=10, padx=10)
s5.grid(row=4, column=0, sticky="nsew")
s5.columnconfigure(0, weight=1, uniform="a")

results_label = tk.Text(s5, font=("Verdana", 13), height=5, bg="white", borderwidth=2, relief="solid")
results_label.insert(tk.END, f"Esperando...\n")
results_label.grid(row=0, column=0, sticky="nsew", pady=(0,10), padx=10)


### MANEJADOR DE SEÑALES ###

signal.signal(signal.SIGINT, funcion_int)
signal.signal(signal.SIGCONT, event_start)
signal.signal(signal.SIGXCPU, event_start)
signal.signal(signal.SIGXFSZ, event_start)
signal.signal(signal.SIGPIPE, event_end)
signal.signal(signal.SIGVTALRM, event_end)
signal.signal(signal.SIGPROF, event_end)

### CIERRE DE LA VENTANA ###

window.protocol("WM_DELETE_WINDOW", on_closing)

### DESPLEGAR VENTANA ###

window.mainloop()
