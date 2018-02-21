import serial
from flask import Flask
import json

######VARIABLES######
class informacion:
    status = "Unknown"
    fecha = '00/00/00'
    hora = '00:00:00'
    temperatura = 0.0
    presion = 0.0
    humedad = 0.0
    velViento = 0.0
    precipitacion = 0.0
    dirViento = 'N/A'

    estado = False
#Instancia
info = informacion()

######INICIALIZACION######

app = Flask(__name__)
#no se puede devolver variables tipo dato en las funciones
#devolvemos un JSON

######FUNCIONES######
def secureData (serial):#cuenta que existan los caracteres referencia para
#considerar el string de datos como no corrupto
    counter=0#contador del parametro referencia en el envio de datos (#)
    for x in serial:
        if(x=='#'):
            counter+=1
    return counter
def splitData(data_str):
    datawjunk=data_str.partition("######")#3 particiones se generan
    #la ultima particion data[2] contiene lo existente a la derch. de ######
    datanojunk=datawjunk[2].split("_")#divide los datos eliminando las _
    return datanojunk

def getData():
    rfile = open("DATA.txt","r")
    data_list = rfile.readlines()#lista con cada linea del fichero en string como elemento de la lista
    if not data_list:
        return
    last_line = data_list[len(data_list) - 1]
    rfile.close()

    ref_counter=secureData(last_line)
    if(ref_counter>=6):#si recibe las 6 # procede a captar los datos
        ref_counter=0
        data=splitData(last_line)

        info.fecha=data[0]
        info.hora=data[1]
        info.temperatura=float(data[3])
        info.presion=float(data[5])/100
        info.humedad=float(data[7])
        info.velViento=float(data[9])
        info.precipitacion=float(data[11])
        dirViento_junk2=data[13]

        #dirViento llega con \n en su string, eliminamos
        dirViento_junk1 = dirViento_junk2.partition("\n")
        info.dirViento = dirViento_junk1[0]

        #status
        st_file = open("STATUS.txt","r")
        info.status = st_file.read()
        st_file.close()

def printData():#funcion para el desarrollo
    print "Estado: %s" % info.status
    print "Fecha: %s" % info.fecha
    print "Hora: %s" % info.hora
    print "Temperatura: %s grados" % info.temperatura
    print"Presion: %s kPa" % info.presion
    print"Humedad: %s %%" % info.humedad
    print"Precipitacion: %s mm/s" % info.precipitacion
    print"Velocidad viento: %s Km/h" % info.velViento
    print"Direccion viento: %s" % info.dirViento

######TEST######
getData()
printData()
######END OF TEST######

######LOOP######
@app.route("/api/estacion/montegancedo")#por defecto el metodo para acceder a la ruta es GET
def sendData():
    getData()
    #datos listos para enviar
    datos={"Estacion":{"Estado":info.status,
        "Fecha":info.fecha,
        "Hora":info.hora,
        "Temperatura":info.temperatura,
        "Presion":info.presion,
        "Humedad":info.humedad,
        "Precipitacion":info.precipitacion,
        "Velocidad viento":info.velViento,
        "Direccion viento":info.dirViento}}
    return json.dumps(datos)

#@app.route('/api/estacion/montegancedo/alarm')   
def start():
    app.run(host='0.0.0.0',port=5000)
    
if __name__ == "__main__":
    #app.run(host='127.0.0.1',port=5000)
    start()
