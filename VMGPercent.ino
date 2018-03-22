#include <Adafruit_GPS.h> //Load the GPS Library. Make sure you have installed the library form the adafruit site above
#include <SoftwareSerial.h> //Load the Software Serial Library. This library in effect gives the arduino additional serial ports
#include <math.h>
SoftwareSerial mySerial(3, 1); //Initialize SoftwareSerial, and tell it you will be connecting through pins 2 and 3
Adafruit_GPS GPS(&mySerial); //Create GPS object
#include <LiquidCrystal.h> // load ligquid crystal library
LiquidCrystal LCD(10,9,5,4,6,7);//load liquid crystal object named LCD
//float mph=1.15078;//convert knots to mph
String NMEA1;  //We will use this variable to hold our first NMEA sentence
String NMEA2;  //We will use this variable to hold our second NMEA sentence
char c;       //Used to read the characters spewing from the GPS module 
double lat1;//current latitiude
double lon1;//current longitude
double lat2;
double lon2;
double latR1;
double latR2;
double lonR1;
double lonR2;
double dlon;
double dlat;
double x;
double y;
double a;
double e;
double d;
double R = 6371000.;
double toDegrees = 57296.;
float deg; //Will hold positin data in simple degree format
float degWhole; //Variable for the whole part of position 
float degDec;  //Variable for the decimal part of degree
double brngR; //bearing variable in radians
int brng;// bearign variable in decdeg
int buttonPin = 2;//the pin that the button is attached to
int state = 0;//the counter for which mark and its latlon
char marks[7][1]={"D","B","3","4","5","6","7"};
double latlongs[7][2]={
  {30.44316,-97.92153},
  {30.43713,-97.92425},
  {30.41046,-97.8864},
  {30.40069,-97.90274},
  {30.40751,-97.90812},
  {30.42208,-97.90536},
  {30.43514,-9792063}};
void next(){//interrupt function for changing which mark we are heading to and it's latlon
  if (state>5){
    state=0; 
  }
  else{
    state=state+1;
  }
 }
void setup() {
  // put your setup code here, to run once:
GPS.begin(9600);//turn on GPS @9600 baud
GPS.sendCommand("$PGCMD,33,0*6D");//turn off GPS antenna signal
GPS.sendCommand("PMTK_SET_NMEA_UPDATE_1HZ");//set update rate to 1Hz
GPS.sendCommand("PMTK_SET_NMEA_OUTPUT_RMCGGA");//request rmc and gga sentences only
delay(1000);//wait 1 second before trying to do anythign else
pinMode(buttonPin, INPUT_PULLUP);
attachInterrupt(0, next,FALLING);
LCD.begin(16,2);// tell arduino our lcd has 16 columns and 2 rows
LCD.setCursor(1,0);// tell LCD to start at 0,0
LCD.println("Vel       ");//heading to Velocity
LCD.setCursor(8,0);// tell LCD to start at 0,0
LCD.println("vmg:       ");//heading to VelocityMadeGood
LCD.setCursor(0,1);// tell LCD to start at 0,1
LCD.println("brn:     ");//set second row to AngleToMark
LCD.setCursor(7,1);// tell LCD to start at 7,1 for distance text
LCD.println("dist:  ");//set second row to Distance to mark
}

void loop() {
  lat2=latlongs[state][0];
  lon2= latlongs[state][1];
  readGPS();
  makeLatDD();
  makelonDD();//function to convert gps.lat
  calcDist(); //Call the distance and bearing calculation function
  calcBearing();//call function to calculate bearing to marker
  LCD.setCursor(0,0); //set cursor to first column speed data entry
  LCD.print(marks[state][0]);
  LCD.setCursor(4,0); //set cursor to first column speed data entry
  LCD.print((GPS.speed),2);//output current gps speed in mph
  LCD.setCursor(12,0); //set cursor to first column speed data entry
  LCD.print(GPS.speed*cos((GPS.angle-brng)*(PI/180)),2);//velocity made good toward the given mark
  LCD.setCursor(12,1);// tell LCD to input distance value
  LCD.println(d,2);
  LCD.setCursor(4,1); //set cursor to first column speed data entry
  LCD.println(brng);
}

void readGPS() {//read data from NMEA sentence 
  clearGPS();
  while(!GPS.newNMEAreceived()) { //Loop until you have a good NMEA sentence
    c=GPS.read();
  }
  GPS.parse(GPS.lastNMEA()); //Parse that last good NMEA sentence
  NMEA1=GPS.lastNMEA();
  
   while(!GPS.newNMEAreceived()) { //Loop until you have a good NMEA sentence
    c=GPS.read();
  }
  GPS.parse(GPS.lastNMEA()); //Parse that last good NMEA sentence
  NMEA2=GPS.lastNMEA(); 
  Serial.println(NMEA1);
  Serial.println(NMEA2);
  Serial.println(""); 
} 
void clearGPS() {  //Clear old and corrupt data from GPS serial port 
  while(!GPS.newNMEAreceived()) { //Loop until you have a good NMEA sentence
    c=GPS.read();
  }
  GPS.parse(GPS.lastNMEA()); //Parse that last good NMEA sentence
  
  while(!GPS.newNMEAreceived()) { //Loop until you have a good NMEA sentence
    c=GPS.read();
  }
  GPS.parse(GPS.lastNMEA()); //Parse that last good NMEA sentence
   while(!GPS.newNMEAreceived()) { //Loop until you have a good NMEA sentence
    c=GPS.read();
  }
  GPS.parse(GPS.lastNMEA()); //Parse that last good NMEA sentence  
}

void calcDist(){ //calculate distance from current location to current mark selected
  //This portion converts the current and destination GPS coords from decDegrees to Radians
  lonR1 = lon1*(PI/180);
  lonR2 = lon2*(PI/180);
  latR1 = lat1*(PI/180);
  latR2 = lat2*(PI/180);
  x = (latR1-latR2) * cos((lonR1+lonR2)/2);// formula from http://movable-type.co.uk/scripts/latlong.html
  y = (lonR2-lonR1);
  d = sqrt(x*x + y*y) * R;
  }
void calcBearing(){ //calculate bearing from current location to current mark
  brngR = atan2(sin(lonR2-lonR1)*cos(latR2),cos(latR1)*sin(latR2)-sin(latR1)*cos(latR2)*cos(lonR2-lonR1));
  brng=(brngR * 4068) / 71;//converts radians to dd
  brng=brng+360;
  brng=brng % 360;
  }
  
void makelonDD(){//convert  gps.longitude into decimal degrees
  degWhole=float(int(GPS.longitude/100)); //gives me the whole degree part of Longitude
  degDec = (GPS.longitude - degWhole*100)/60; //give me fractional part of longitude
  deg = degWhole + degDec; //Gives complete correct decimal form of Longitude degrees
  if (GPS.lon=='W') {  //If you are in Western Hemisphere, longitude degrees should be negative
    deg= (-1)*deg;
  }
  
  lon1=deg;
}
void makeLatDD(){//function to convert GPS value to latitiude in Decimal degrees
  degWhole=float(int(GPS.latitude/100)); //gives me the whole degree part of latitude
  degDec = (GPS.latitude - degWhole*100)/60; //give me fractional part of latitude
  deg = degWhole + degDec; //Gives complete correct decimal form of latitude degrees
  if (GPS.lat=='S') {  //If you are in Southern hemisphere latitude should be negative
    deg= (-1)*deg;
  }
  lat1=deg;
}
        
