#coding: utf-8
#Mantiene la escucha constante a traves de serial.
import serial
import serial.tools.list_ports
import time
import sys

from decode import *


csv_file = "weather.csv"
log_file = "weather.txt"

######FUNCTIONS######
def storeStatus(status):
    st_file = open("STATUS.txt","w")
    st_file.write(status)
    st_file.close()

def seleccionar_puerto():
    i = 0
    ports = serial.tools.list_ports.comports()
    if i == 1: #solo un puerto
        return ports[0].device
    for s in ports:
        print("{}: {}\t\t{}".format(i,s.device,s.usb_description()))
        i += 1
    n_port =int( input("Introducir número para seleccionar puerto: "))
    return ports[n_port].device
 
puerto_serie = seleccionar_puerto()

try:
    xbee=serial.Serial(puerto_serie,9600) #ACMx / USBx (x = 1 o 0)
except serial.serialutil.SerialException as s:
    sys.stderr.write("{}\n".format(s))
    exit(-1)

######LOOP######
xbee.flushInput()
print("Esperando paquetes de datos...")
prev_time = time.time()
while True:
    curr_time = time.time()
    #si la espera del serial se alarga el status cambia a No operativo
    if True: #((xbee.inWaiting()) and (curr_time-prev_time<cicle_time)):
        prev_time = curr_time
        try:
            serial_data = (xbee.readline())#recibe el string del objeto serial
        except KeyboardInterrupt:
            print("Saliendo...")
            exit(0)

        ts,weather = decode(serial_data)
        if ts is None or weather is None:
            continue
        log_str = str(ts) + str(weather) + "\n"
        csv_str = ts.csv() + weather.csv() + "\n"
        print "\nPaquete recibido"
        print(log_str)
        wfile = open(log_file,"a+")
        wfile.write(log_str)
        wfile.close()

        wfile = open(csv_file,"a+")
        wfile.write(csv_str)
        wfile.close()
        
        #status
        status = "Operativo"
        storeStatus(status)

       
