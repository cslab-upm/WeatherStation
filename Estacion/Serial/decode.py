#coding: utf-8
class Timestamp:
  def __init__(self,year, month, date, hour, minute, second):
    self.year = year
    self.month = month
    self.date = date
    self.hour = hour
    self.minute = minute
    self.second = second
  def __str__(self):
    return ("[%03d/%02d/%02d %02d:%02d:%02d]" % \
(self.year, self.month, self.date, self.hour, self.minute, self.second))
  def csv(self):
    return ("{},{},{},{},{},{},".format(\
self.year, self.month, self.date, self.hour, self.minute, self.second))

class Weather:
  wind_dirs = ["N","NNE","NE",'ENE','E','ESE','SE','SSE','S','SSW','SW','WSW'\
'W','WNW','NW','NNW',"N/A"]
  def __init__(self,temp,pres,windS,windD,hum,prec):
    self.temp = temp
    self.pres = pres
    self.windS = windS
    self.windD = windD
    self.hum = hum
    self.prec = prec
  def __str__(self):
    return ("""\n\tTemperature: {}ºC
\tPression: {}hPa
\tWind speed: {}km/h
\tWind direction: {}
\tHumidity: {}%
\tPrecipitation: {}mm/min\n""".format(\
self.temp, self.pres, self.windS, Weather.wind_dirs[self.windD], self.hum, self.prec))
  def csv(self):
    return("{},{},{},{},{},{}".format(\
self.temp, self.pres, self.windS, Weather.wind_dirs[self.windD], self.hum, self.prec))

def valid_packet(p):
  if len(p) < 15:
    return False
  return p[0] == ord("#") and p[14] == ord("&")
def decode(packet):
  packet_bytes = [ord(c) & 0xFF for c in packet]
  if not valid_packet(packet_bytes):
    return (None, None)
  
  year = 2000+(((packet_bytes[1] & 0x7f)) | ((packet_bytes[2]&0x7F) >> 5));
  month = (packet_bytes[2] >> 1) & 0xF;
  date = ((packet_bytes[2] & 1) <<4) | (((packet_bytes[3] & 0x7F) >>3) & 0xF);
  hour = ((packet_bytes[3] & 0x7)<<2) | ((packet_bytes[4] >> 5)&0x3);
  minute = ((packet_bytes[4]& 0x1F)<<1 )| ((packet_bytes[5] >> 6)&0x1);
  second = packet_bytes[5] & 0x3F;

  temp = (packet_bytes[6] & 0x7F) - 40 ;
  pres = (((packet_bytes[7] & 0x7F) << 7) | (packet_bytes[8] & 0x7F))/16;

  windVal = (((packet_bytes[9] & 0x7F) << 7) | (packet_bytes[10] & 0x7F));
  windD = windVal >>10;
  windS = (windVal & 0x3FF)/10;

  prec = (((packet_bytes[11] & 0x7F) << 7) | (packet_bytes[12] & 0x7F))/16;
  hum = packet_bytes[13] & 0x7F;
  print("Viento: {}".format(windD))
  return (Timestamp(year,month,date,hour,minute,second),\
Weather(temp,pres,windS,windD,hum,prec))
  
