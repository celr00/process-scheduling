import tkinter as tk
import os

server_pid = 4087

def send_hup():
    os.system("kill -s HUP {}".format(server_pid))


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

btn = tk.Button(window, text='SIGHUP', command=send_hup)
btn.pack()

window.mainloop()
