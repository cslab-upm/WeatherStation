#!/bin/bash

# Crear los ficheros para la comunicación entre procesos
touch DATA.txt
touch STATUS.txt

# Iniciar el cliente serie para comunicarse de forma inalámbrica con la estación meteorológica
echo " [Estacion] Iniciando comunicación con la estación meteorológica"
python Serial/WeatherSerial.py > /dev/null &


# Iniciar el servidor para la API
echo "[Estacion] Iniciando API REST"
python API/WeatherAPIv2.py 
