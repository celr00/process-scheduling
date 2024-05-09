import tkinter as tk
import os
import signal
import sys

def signal_handler(sig, frame):
    label1.config(text="Estatus: EJECUTANDO")

def send_hup():
    server_pid = server_pid_txt.get()
    os.system("kill -s HUP {}".format(server_pid))

# TO EXIT: sys.exit(0)

window = tk.Tk()
window.title("Cliente con PID {}".format(os.getpid()))

# Obtener las dimensiones de la pantalla
ancho_pantalla = window.winfo_screenwidth()
alto_pantalla = window.winfo_screenheight()

# Calcular las coordenadas para centrar la window
ancho_window = 500
alto_window = 300
posicion_x = (ancho_pantalla - ancho_window) // 2
posicion_y = (alto_pantalla - alto_window) // 2

# Establecer el tamaño y la posición de la window
window.geometry(f"{ancho_window}x{alto_window}+{posicion_x}+{posicion_y}")

server_pid_txt = tk.Entry(window, width=30)
server_pid_txt.pack()

btn = tk.Button(window, text='SIGHUP', command=send_hup)
btn.pack()

label1 = tk.Label(window, text="Estatus: ")
label1.pack()

signal.signal(signal.SIGUSR2, signal_handler)

window.mainloop()
