#include <string.h>
#include "encode.h"
struct timestamp;
struct weather;
/*  Temperature
 * =============
 * Since our temperature sensor has a limited accuracy and there
 * is no difference for us in half a degree, we will set a resolution
 * of one degree for our temperature field, which can be between 
 * -40 and 85 ºC, and therefore can be encoded in 7 bits forcing the
 * most significant bit of the byte to a logic '1' to avoid null character.
 * This means that the temperature field will be one byte long with
 * the following structure:
 *
 *   1 t t t t t t t
 *
 * where ttttttt is a 7-bit integer which can be from 0 (meaning
 * a temperature of -40ºC) to 127 (meaning a temperature of 87ºC)
 * 
 *  Pressure
 * ==========
 * Typically, pressure values go from 885 hPa to 1077 hPa [Wikipedia:
 * https://es.wikipedia.org/wiki/Altas_y_bajas_presiones], which are
 * 192 different integer values. It is not possible to encode that
 * in 7 bits so we will use 14 bits in 2 bytes, allowing us to have
 * a resolution of 1/16 hPa by havind the following structure:
 *
 * 1 p p p p p p p 1 p p p p p p p 
 * 
 * where ppppppppppp.pppp is a 14-bit number with 10 bits integer and
 * 4 bits decimal wich can be from 0 to 2047.9375 hPa
 *
 *  Humidity
 * ==========
 * In this case, relative humidity is measured as a percentage, so an
 * integer between 1 and 99 will be enough, following the same structure
 * as the temperature but with 0 meaning 0% and 100 meaning 100%
 *
 *  Wind Status
 * =============
 * Wind speed and direction will be coded as:
 * 
 * 1 d d d d s s s 1 s s s s s s s
 *
 * where ssssssssss is a 10-bit number which is 10 times the wind speed in
 * km/h, allowing a representation interval between 0 and 204,7 km/h; and
 * dddd encondes the distance in 4 bits:
 *
 *    | dddd | Direction |
 *    |------+-----------|
 *    | 0x0  |     N     |
 *    | 0x1  |    NNE    |
 *    | 0x2  |    NE     |
 *    | 0x3  |    ENE    |
 *    | 0x4  |     E     |
 *    | 0x5  |    ESE    |
 *    | 0x6  |    SE     |
 *    | 0x7  |    SSE    |
 *    | 0x8  |     S     |
 *    | 0x9  |    SSW    |
 *    | 0xA  |    SW     |
 *    | 0xB  |    WSW    |
 *    | 0xC  |     W     |
 *    | 0xD  |    WNW    |
 *    | 0xE  |    NW     |
 *    | 0xF  |    NNW    |
 *
 *  Precipitation
 * ===============
 * We will measure precipitation in mm per minute and will use 14 bits to
 * encode it in the same way as pressure.
 * 
 *  Timestamp
 * ===========
 * In our timestamp we will have year(y), month(M), day(d), hour(h), minute(m)
 * and second fields. Obviously, we can encode month in 4 bits, day and our 
 * in 5, minute and second in 6. That makes 26 bits and, since we are using
 * groups of 7 bits, it seems logical to use 5 bytes for timestamp and use the
 * 9 remaining bits for the year, starting from 2000. This way:
 *
 *   1yyyyyyy|1yyMMMMd|1ddddhhh|1hhmmmmm|1mssssss
 *
 *  Full packet
 * =============
 * In an arbitrary way, we will order the fields as follows:
 * 
 *   #|Timestamp|Temperature|Pressure|Wind|Precipitation|Humidity|&
 *
 * '#' and '&' are control characters to denote begining and end of message.
 *  We have, thus, a packet with a fixed length of 15 bytes. 
 */
void packet_timestamp(struct timestamp* ts, char* packet);
void packet_bytes(int value, int n_bytes, char* packet);
/** PROC gen_packet
 * Creates a packet for sending through serial or radio
 * PARAMS: timestamp ts: the time and date of the data
 *         weather    w: the data about weather status
 *         char* packet: output param which must be at least 16 bytes long.
 */
void gen_packet(struct timestamp* ts, struct weather* w, char* packet){
  // Force all most significant bytes to 1
  int i;
  for(i = 0; i< 16; i++)
    packet[i] = (char) 0x80;
  // Add control characters
  packet[0] = '#';
  packet [14] = '&';
  packet [15] = '\0';
  // Encode timestamp in bytes 1-5
  packet_timestamp(ts, &packet[1]);
  // Encode temperature in byte 6
  int myTemp; // Actual temperature plus 40 to get from -40 to 87 with positives
  myTemp = w->temp + 40;
  packet_bytes(myTemp, 1, &packet[6]);
  // Encode pressure in bytes 7-8
  int myPress; // Actual pressure coded in fixed point position
  myPress = (int) (w->pres * 16);
  packet_bytes(myPress,2,&packet[7]);
  // Encode wind in bytes 9-10 
  int windVal; // All wind information
  windVal = (w->windD << 10) | (((int)(w->windS * 10)) & 0x3FF);
  packet_bytes(windVal, 2, &packet[9]);
  // Encode precipitation in bytes 11-12
   int myPrec; // Actual precipitation amount coded in fixed point position
  myPrec = (int) (w->prec * 16);
  packet_bytes(myPrec,2,&packet[11]);
  // Encode humidity in byte 13;
   packet_bytes(w->hum, 1, &packet[13]);
  // Done
}

void packet_timestamp(struct timestamp* ts, char* packet){
  // -----------Encode timestamp-----------
  //    --Encode year--
  int myYear; // Actual year minus 2000 to be able to encode in 9 bits
  myYear = ts->year - 2000;
  packet[0] |= (myYear >> 2);  //7 most signifcant bits
  packet[1] |= (myYear & 0x3)<<5; //2 least significant bits
 
 //     --Encode month--
  packet[1] |= ((ts->month) & 0xF) << 1; // positions 1-4 of the byte
 //     --Encode day--
  packet[1] |= (ts->date >> 4) & 0x1; // Most signifcant bit only 
  packet[2] |= (ts->date & 0xF) << 3;   // 4 remaining bits to positions 3-6
 //     --Encode hour--
  packet[2] |= (ts->hour >> 2) & 0x7; // 3 most  significant bits
  packet[3] |= (ts->hour & 0x3) << 5; // 2 remaining bits to positions 5-6
 //     --Encode minute--
  packet[3] |= (ts->minute >> 1) & 0x1F; // 5 most significant bits
  packet[4] |= (ts->minute & 0x1) << 6;  // remaining bit to position 6
 //     -- Encode second-- 
  packet[4] |= (ts->second & 0x3f); // all 6 bits
}
/** PROC packet_bytes encodes given value in the 7 least significant bits
  * of each byte of the first n_bytes of packet
  */
void packet_bytes(int value, int n_bytes, char* packet){
  int i;
  int byte_value; // value to encode in each byte
  for(i=n_bytes-1;i>=0;i--){
    byte_value = (value >> (7*i)) & 0x7F;
   
    packet[n_bytes - (1+i)] |= byte_value;
  }
}
