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
  int hum;   // humidity
  float prec;  // precipitation
  int windD;   // wind direction
};


void gen_packet(struct timestamp*, struct weather*, char* packet);
