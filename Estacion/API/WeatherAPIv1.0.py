import serial
from flask import Flask
import json

######INICIALIZACION######
xbee=serial.Serial('/dev/ttyUSB0',9600)
app = Flask(__name__)
#no se puede devolver variables tipo dato en las funciones
#devolvemos un JSON

######FUNCIONES######
def secureSerialData (serial):#cuenta que existan los caracteres referencia para
#considerar el string de datos como no corrupto
    counter=0#contador del parametro referencia en el envio de datos (#)
    for x in serial:
        if(x=='#'):
            counter+=1
    return counter
def getSerialData(data_str):
    datawjunk=data_str.partition("######")#3 particiones se generan
    #la ultima particion data[2] contiene lo existente a la derch. de ######
    datanojunk=datawjunk[2].split("_")#divide los datos eliminando las _
    return datanojunk

def printData():
    print "Hora: %s" % hora
    print "Temperatura: %s grados" % temperatura
    print"Presion: %s kPa" % presion
    print"Humedad: %s %%" % humedad
    print"Precipitacion: %s mm/s" % precipitacion
    print"Velocidad viento: %s Km/h" % velViento
    print"Direccion viento: %s" % dirViento

######LOOP######
@app.route("/api/estacion")#por defecto el metodo para acceder a la ruta es GET
def getData():#toma los datos desde el Arduino

    while True:
        serial_data = (xbee.readline())#recibe el string del objeto serial
        ref_counter=secureSerialData(serial_data)
        if(ref_counter>=6):#si recibe las 6 # procede a captar los datos
            ref_counter=0
            print(serial_data)
            data=getSerialData(serial_data)

            hora=data[0]
            temperatura=data[2]
            presion=data[4]
            humedad=data[6]
            velViento=data[8]
            precipitacion=data[10]
            dirViento=data[12]

            #printData()
            datos={"Estacion":[{"Informacion":[{"Dato":"Hora","Valor":hora},{"Dato":"Temperatura","Valor":temperatura},{"Dato":"Presion","Valor":presion},{"Dato":"Humedad","Valor":humedad},{"Dato":"Precipitacion","Valor":precipitacion},{"Dato":"Velocidad viento","Valor":velViento},{"Dato":"Direccion viento","Valor":dirViento}]}]}
            return json.dumps(datos)

        else:#en caso de no recibir la referencia rechaza
            #los datos por posible corrupcion
            return "Datos incompletos, espere al siguiente paquete"
            continue



if __name__ == "__main__":
    app.run(host='127.0.0.1',port=5000)
