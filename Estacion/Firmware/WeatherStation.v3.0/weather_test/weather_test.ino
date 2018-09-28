/*
File for testing all different components 
*/

// RTC 
#include <SparkFunDS1307RTC.h>
#include <Wire.h>

// Termometer/barometer
#include <Adafruit_BMP085.h>

// Hygrometer
#include <hh10d.hpp>
const int HH10D_OFFSET(7732);
const int HH10D_SENSITIVITY(402);
float hum;

// Sparkfun station
#define ANEM_PIN 2 //anemometer
#define PLUV_PIN 3 //pluviometer
#define VANE_PIN 4 //wind vane
#define RESET_PIN 14

volatile int anem_ticks;//Amount of anemometer ticks measured
volatile int pluv_ticks;//Amount of pluviometer ticks measured

float windS = 0.;
float prec = 0.;
int windD;

//Interrupts
//#include <avr/interrupts.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include "timers.h"

// Radio communication (XBEE)
#include "encode.h"
char packet[16];

// Data structures
struct timestamp ts;
struct weather w;

// Number of interrupt cycles to be asleep
#define SLEEP_CYCLES 1 // about 28 seconds
#define SLEEP_DURATION 4.09 //seconds
volatile int cycles_so_far = 0;
//Function declarations
void readTime(struct timestamp*); 
void setupRTC(void);
void printTime(struct timestamp);

void setupBMP(void);
void readBMP(int*,float*);
void printBMP(int,float);

void setupHH10D();
void readHum(int*);
void printHum(int);

void setupStation();
void readStation(float*, float*, int*);
void printStation(float, float, int);

void setup(){
   //cli();
  Serial.begin(57600);
  Serial.print("Setting up RTC...");
  setupRTC();
  Serial.print("OK.\nSetting up BMP...");
  setupBMP();
  Serial.print("OK.\nSetting up HH10D...");
  setupHH10D();
  Serial.print("OK.\nSetting up Spark Station...");
  setupStation();
  Serial.print("OK.\nPreparing Interrupts...");
  periodical_interrupt();
  Serial.println("OK.");
  // sei();
}
volatile int awake = 1;
// RTC variables
int time[6];

// BMP variables
Adafruit_BMP085 bmp;
float temp = 0.0;
float pres = 0.0;

void loop(){
/*
 * Read the time every second and print it through the serial 
 */
   // Format: yy/mm/dd hh:mm:ss
  if(awake){
   //cli();
  awake = 0;
  readTime(&ts);
  readStation(&(w.prec),&(w.windS),&(w.windD));
  readBMP(&(w.temp),&(w.pres));
  readHum(&(w.hum));
  printTime(ts);
  printBMP(w.temp,w.pres);
  printHum(w.hum);
  printStation(w.prec,w.windS,w.windD);
//  sei();
  }
  enter_sleep();
  //delay(1500);
}

ISR(TIMER1_OVF_vect){
//  sleep_disable();
  cycles_so_far = (cycles_so_far + 1) % SLEEP_CYCLES;
  if(!cycles_so_far){ // cycles == 0
    awake = 1;
  }
}
void setupRTC(){
   rtc.begin(); // Initialize RTC
   //rtc.autoTime(); // Set time. Comment when properly configured

}
void readTime(struct timestamp* t){
  rtc.update(); // Update rtc data
  t->second= rtc.second(); // Get rtc second
  t->minute = rtc.minute(); // Get rtc minute
  t->hour = rtc.hour(); // Get rtc hour
  t->date = rtc.date(); // Get rtc day
  t->month= rtc.month(); // Get rtc month
  t->year = rtc.year(); // Get rtc year
   
   //return time;
}

void printTime(struct timestamp t){
   // PRE: time has at least 6 positions
   String timeStr=String() ;
   timeStr += "["+String(t.year) + "/" + String(t.month) + "/" +
              String(t.date) + " " + String(t.hour) + ":" +
              String(t.minute) + ":" + String(t.second) + "]";

   Serial.print(timeStr);

}

void setupBMP(){
  bmp.begin();
}
void readBMP(int* temp, float* pres){
  *temp = bmp.readTemperature();//ºC
  *pres = bmp.readPressure()/100; // convert Pa to hPa
}
void printBMP(int temp, float pres){
  String bmpStr= String();
  bmpStr += "t = " + String(temp) + "ºC, p = " + String(pres) + "hPa, ";

  Serial.print(bmpStr);

}

void setupHH10D(void){
  HH10D::initialize();
}
void readHum(int* hum){
  //HH10D::initialize();
  *hum =(int) HH10D::getHumidity(HH10D_SENSITIVITY, HH10D_OFFSET);
}
void printHum(int hum){
  String humStr = String();
  humStr += "h = " + String(hum) + "% ,";
  Serial.print(humStr);
}

void anem_interrupt(){
  anem_ticks++; 
}
void pluv_interrupt(){
  pluv_ticks++;
}
void setupStation(){
  pinMode(ANEM_PIN, INPUT);
  pinMode(PLUV_PIN,INPUT);
  pinMode(VANE_PIN,INPUT);
  pinMode(RESET_PIN,OUTPUT);
  digitalWrite(ANEM_PIN,HIGH);//Pull-up
  attachInterrupt(digitalPinToInterrupt(2), anem_interrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), pluv_interrupt, FALLING);
  digitalWrite(RESET_PIN,HIGH);
}
/** PROC wind_dir
 *  PARAMS INT measurement: 0 to 1023 integer from adc of sparkfun 
 *                          wind vane.
 *  Returns Int representing direction
 */
int wind_dir(int measurement ){
  float voltage = measurement * (5.0 / 1023);
  int result;
  // Since there is no documentation about how the vane is connected,
  // we will use experimental values
 if(voltage>=3.80&&voltage<=3.87){
    result=0x0;//"N";
  }
  else if(voltage>=3.40&&voltage<=3.47){
    result=0xF;//"NNW";
  }
  else if((voltage>=4.31)&&(voltage<=4.36)){
    result=0xE;//"NW";
  }
  else if((voltage>=4.02 )&&(voltage<= 4.07)){
    result=0XD;//"WNW";
  }
  else if(voltage>=4.58&&voltage<=4.64){
    result=0xC;//"W";
  }
  else if((voltage>=2.91 )&&(voltage<=2.96 )){
    result=0xB;//"WSW";
  }
  else if(voltage>=3.05&&voltage<=3.11){
    result=0xA;//"SW";
  }
  else if((voltage>=1.18 )&&(voltage<= 1.22)){
    result=0x9;//"SSW";
  }
  else if((voltage>=1.39 )&&(voltage<=1.43 )){
    result=0x8;//"S";
  }
  else if((voltage>= 0.60)&&(voltage<=0.64 )){
    result=0x7;//"SSE";
  }
  else if((voltage>= 0.88)&&(voltage<=0.92 )){
    result=0x6;//"SE";
  }
  else if((voltage>=0.30 )&&(voltage<=0.34 )){
    result=0x5;//"ESE";
  }
  else if(voltage>=0.43&&voltage<=0.47){
    result=0x4;//"E";
  }
  else if((voltage>=0.39 )&&(voltage<=0.42 )){
    result=0x3;//"ENE";
  }
  else if((voltage>= 2.24)&&(voltage<=2.29 )){
    result=0x2;//"NE";
  }
  else if((voltage>=1.97)&&(voltage<=2.01)){
    result=0x1;//"NNE";
  }
  else{
    result=-1;//"N/A";
    Serial.println("\n\nERROR: voltaje = " + String(voltage));
  }
  return result;
}

void readStation(float* prec, float* windS, int*windD){
  int vane = analogRead(VANE_PIN);
  *windS = (anem_ticks/(SLEEP_CYCLES*SLEEP_DURATION))*2.4;//km/h
  *prec =  (pluv_ticks/(SLEEP_CYCLES*SLEEP_DURATION))*60*0.2794; //mm/min
  *windD = wind_dir(vane);
  //Reset counters
  anem_ticks = 0;
  pluv_ticks = 0;
}
void printStation(float prec, float windS, int windD){
  String sparkStr= String();
  sparkStr += "prec = " + String(prec) + "mm/min, ws = " + String(windS) + "km/h, wd = " + 
               String(windD);

  Serial.println(sparkStr);


}
