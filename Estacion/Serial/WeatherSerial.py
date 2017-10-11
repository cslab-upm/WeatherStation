#Mantiene la escucha constante a traves de serial.
import serial
import time
sec_counter = 6 #security counter
extension = ".txt"
mes = "00"
anio = "00"
status = "Operativo"
prev_time = 0.0
curr_time = 0.0
cicle_time = 180 #180 segundos de espera hasta cambiar el estado a No Operativo
######LINUX######
xbee=serial.Serial('/dev/ttyUSB1',9600) #ACMx / USBx (x = 1 o 0)

######WINDOWS#####
#xbee=serial.Serial('COM7',9600) #comprobar el numero de COM

######FUNCTIONS######
def secureSerialData (serial):#cuenta que existan los caracteres referencia para
#considerar el string de datos como no corrupto
    counter=0#contador del parametro referencia en el envio de datos (#)
    for x in serial:
        if(x=='#'):
            counter+=1
        if(counter>=sec_counter):
            break
    return counter

"""def splitData(data_str):
    datawjunk=data_str.partition("######")#3 particiones se generan
    #la ultima particion data[2] contiene lo existente a la derch. de ######
    datanojunk=datawjunk[2].split("_")#divide los datos eliminando las _
    return datanojunk"""

def storeStatus(status):
    st_file = open("STATUS.txt","w")
    st_file.write(status)
    st_file.close()

######LOOP######
xbee.flushInput()
print("Esperando paquetes de datos...")
prev_time = time.time()
while True:
    curr_time = time.time()
    #si la espera del serial se alarga el status cambia a No operativo
    if((xbee.inWaiting()) and (curr_time-prev_time<cicle_time)):
        prev_time = curr_time

        serial_data = (xbee.readline())#recibe el string del objeto serial
        serial_data = serial_data.partition("//////")
        package = serial_data[2]
        print "\nPaquete recibido"
        print(package)
        if(secureSerialData(package)>=sec_counter):
            wfile = open("DATA.txt","a+")
            wfile.write(package)
            wfile.close()

            #status
            status = "Operativo"
            storeStatus(status)

            print "Datos validos y guardados"
        else:
            print "Datos invalidos"
    elif(curr_time-prev_time>=cicle_time):
        prev_time = curr_time
        status = "No operativo"
        storeStatus(status)
