#!/bin/bash

# Crear los ficheros para la comunicación entre procesos
touch DATA.txt
touch STATUS.txt

# Iniciar el cliente serie para comunicarse de forma inalámbrica con la estación meteorológica
echo " [Estacion] Iniciando comunicación con la estación meteorológica"
python Serial/WeatherSerial.py  & pid_serial=$!

echo "PID del monitor serial: $pid_serial"

# Iniciar el servidor para la API
echo "[Estacion] Iniciando API REST"
python API/WeatherAPIv2.py & pid_api=$!

echo "PID del servidor de la api: $pid_api"

wait $pid_serial

ret_serial=$?

if [ $ret_serial -ne 0 ];then
    kill $pid_api
fi