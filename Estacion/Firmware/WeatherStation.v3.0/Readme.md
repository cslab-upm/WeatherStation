# Weather station firmware
## Components
- [Arduino Mega](https://store.arduino.cc/usa/arduino-mega-2560-rev3)
- [SparkFun Weather Meters](https://cdn.sparkfun.com/assets/8/4/c/d/6/Weather_Sensor_Assembly_Updated.pdf "Weather Meters datasheet")
- [DS1307 RTC]( https://datasheets.maximintegrated.com/en/ds/DS1307.pdf "DS1307 datasheet")
- [HH10D Humidity Sensor](https://www.sparkfun.com/datasheets/Sensors/Temperature/HH10D.pdf "hh10d datasheet")
- [BMP085 Temperature and pressure sensor](https://www.sparkfun.com/datasheets/Components/General/BST-BMP085-DS000-05.pdf "BMP085 datasheet")
## Encoding
In order to reduce the packet size for radio communication (through xbee),
we have decided to encode all the information in a 15-byte packet with the
following fields:

'''
Byte  0 1         6           7        9    11            13       14
Field #|Timestamp|Temperature|Pressure|Wind|Precipitation|Humidity|&
'''
### Timestamp 
In our timestamp we will have year(y), month(M), day(d), hour(h), minute(m)
and second fields. Obviously, we can encode month in 4 bits, day and our 
in 5, minute and second in 6. That makes 26 bits and, since we are using
groups of 7 bits, it seems logical to use 5 bytes for timestamp and use the
9 remaining bits for the year, starting from 2000. This way:

'''  
  1yyyyyyy|1yyMMMMd|1ddddhhh|1hhmmmmm|1mssssss
'''

### Temperature

Since our temperature sensor has a limited accuracy and there
is no difference for us in half a degree, we will set a resolution
of one degree for our temperature field, which can be between 
-40 and 85 ºC, and therefore can be encoded in 7 bits forcing the
most significant bit of the byte to a logic '1' to avoid null character.
This means that the temperature field will be one byte long with
the following structure:

'''
  1 t t t t t t t
''' 
where ttttttt is a 7-bit integer which can be from 0 (meaning
a temperature of -40ºC) to 127 (meaning a temperature of 87ºC)
### Pressure
Typically, [pressure values go from 885 hPa to 1077 hPaa](https://es.wikipedia.org/wiki/Altas_y_bajas_presiones), which are
192 different integer values. It is not possible to encode that
in 7 bits so we will use 14 bits in 2 bytes, allowing us to have
a resolution of 1/16 hPa by havind the following structure:
'''
  1 p p p p p p p 1 p p p p p p p 
''' 
 where ppppppppppp.pppp is a 14-bit number with 10 bits integer and
 4 bits decimal wich can be from 0 to 2047.9375 hPa

### Wind

 Wind speed and direction will be coded as:
``` 
  1 d d d d s s s 1 s s s s s s s
```
where ssssssssss is a 10-bit number which is 10 times the wind speed in
km/h, allowing a representation interval between 0 and 204,7 km/h; and
 dddd encondes the distance in 4 bits:

    | dddd | Direction |
    |------|-----------|
    | 0x0  |     N     |
    | 0x1  |    NNE    |
    | 0x2  |    NE     |
    | 0x3  |    ENE    |
    | 0x4  |     E     |
    | 0x5  |    ESE    |
    | 0x6  |    SE     |
    | 0x7  |    SSE    |
    | 0x8  |     S     |
    | 0x9  |    SSW    |
    | 0xA  |    SW     |
    | 0xB  |    WSW    |
    | 0xC  |     W     |
    | 0xD  |    WNW    |
    | 0xE  |    NW     |
    | 0xF  |    NNW    |

### Precipitation
 We will measure precipitation in mm per minute and will use 14 bits to
 encode it in the same way as pressure.
### Humidity 
 In this case, relative humidity is measured as a percentage, so an
 integer between 1 and 99 will be enough, following the same structure
 as the temperature but with 0 meaning 0% and 100 meaning 100%

