# WeatherStation
## API REST
- `GET /api/estacion/montegancedo`

### `GET /api/estacion/montegancedo`
Get information about the current weather.
Response example:
~~~json
{
  "Estacion": {
    "Hora": "17:22:23",
    "Temperatura": 19.5,
    "Velocidad viento": 6.48,
    "Presion": 920.92,
    "Humedad": 16.29,
    "Fecha": "22/02/18",
    "Direccion viento": "N",
    "Estado": "Operativo",
    "Precipitacion": 0.0
  }
}
~~~
The response follows [this schema](Estacion/API/estacion_schema.json).
