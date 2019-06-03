#include <SoftwareSerial.h>
#include <TinyGPS.h>
//#include <PCD8544.h>
#include <SD.h> //CS-D10, MOSI-D11, MISO-D12, SCK-D13, GND — GND, 5V — VCC (опционально в моем случае, в некоторых при отсутствии преобразователя подключаем на 3.3В)
File GPS_file;
TinyGPS gps;
SoftwareSerial gpsSerial(2, 8);//RX - 8 pin, TX - 2 pin

//static PCD8544 lcd; //RST - D6, CE - D7, DC - D5, DIN - D4, CLK - D3, VCC - 5V (опционально, при наличии преобразователя на 3.3В линии), Light - GND, GND - GND
bool newdata = false;
unsigned long start;
long lat, lon;
unsigned long time, date;


void setup() {
  
//lcd.begin(84, 48);
gpsSerial.begin(9600);
Serial.begin(9600);

pinMode(10, OUTPUT);
if (!SD.begin(10)){
//lcd.setCursor(0, 0);
Serial.println("SD-");
return;}
//lcd.setCursor(0, 0);
/*Serial.println("SD+");
GPS_file = SD.open("GPSLOG.txt", FILE_WRITE);
if (GPS_file){
Serial.print("Writing to test.txt...");
GPS_file.print("LATITUDE");
GPS_file.print(",");
GPS_file.print("LONGITUDE");
GPS_file.print(",");
GPS_file.print("DATE");
GPS_file.print(",");
GPS_file.print("TIME");
GPS_file.print(",");
GPS_file.print("ALTITUDE");
GPS_file.println();
GPS_file.close();
Serial.println("done.");
}else{
    Serial.println("error opening test.txt");
}

//lcd.setCursor(0,3);
Serial.print("ALT: ");
//lcd.setCursor(0,2);
Serial.print("SPD: ");
//lcd.setCursor(0,4);
Serial.print("LAT: ");
//lcd.setCursor(0,5);
Serial.print("LON: ");
} */ 
void loop() {
if (millis() - start > 1000){
newdata = readgps();
if (newdata){
start = millis();
gps.get_position(&lat, &lon);
gps.get_datetime(&date, &time);

//lcd.setCursor(50,1);
Serial.print(date);
//lcd.setCursor(55,0);
Serial.print(time*0.000001+5);
//lcd.setCursor(22, 4);
Serial.print(lat);
//lcd.setCursor(22, 5);
Serial.print(lon);
//lcd.setCursor(22, 2);
Serial.print(gps.f_speed_kmph());
//lcd.setCursor(22, 3);
Serial.print(gps.f_altitude());
}
}
GPS_file = SD.open("GPSLOG.txt", FILE_WRITE);
if(GPS_file){
GPS_file.print(lat);
GPS_file.print(",");
GPS_file.print(lon);
GPS_file.print(",");
GPS_file.print(date);
GPS_file.print(",");
GPS_file.print(time*0.000001+5);
GPS_file.print(",");
GPS_file.print(gps.f_altitude());
GPS_file.println();
GPS_file.close();
}else{
  //lcd.setCursor(0, 0);
Serial.println("SD-");
}
}
bool readgps(){
while (gpsSerial.available()){
int b = gpsSerial.read();
if('\r' != b){
if (gps.encode(b))
return true;}}
return false;}
