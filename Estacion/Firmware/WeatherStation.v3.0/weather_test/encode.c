struct timestamp{
  int date;
  int month;
  int year;
  int hour;
  int minute;
  int second;
};
struct weather{
  int temp;  // temperature
  float pres;  // pressure
  float windS; // wind speed
  float hum;   // humidity
  float prec;  // precipitation
  int windD;   // wind direction
};
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
 */
void gen_packet(struct timestamp, struct weather, char* packet);
void gen_log(struct timestamp, struct weather, char* log);
