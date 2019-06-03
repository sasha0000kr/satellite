/*
  Версия 1.5.8 Александр Краснов 2019 ©
  =====================================================================
  Использованы библиотеки:
  SD-Встроена в Arduino IDE
  TinyGPS-https://github.com/mikalhart/TinyGPS
  SoftwareSerial-https://github.com/PaulStoffregen/SoftwareSerial
  iarduino_Pressure_BMP-https://iarduino.ru/file/227.html

  Компоненты:
  Aduino nano (atmega 328p)



  GPS NEO 6m
  Sim 800L
  MicroSD moudle
  BMP280 (BME 280)
  Li-Po или Li-Ion батарея 1S (3.7V)

  Подробная документация по ссылке: https://drive.google.com/drive/folders/1oDss0CeszFDqV4HxMm4E3XqbYwwS1l5c
  ============================================================================================================
  SD:
  CS-D10
  MOSI-D11
  MISO-D12
  SCK-D13
  GND-GND
  VCC-5V (Если питание осуществляется на 3.3v, то через преобразователь)

  Подойдет любой модуль для SD карты.
  Карту форматировать в FAT32 с таблцей msdos.
  ----------------------------------------------------------------------
  GPS NEO 6M:
  RX-D8
  TX-D2
  GND-GND
  VCC-5V (Если питание осуществляется на 3.3v, то через преобразователь)
  ----------------------------------------------------------------------
  SIM800L:
  GND-Общая земля ардуино(GND) и земля линии питания 3.7V От батареи
  TX-D4
  RX-D5
  VCC-Питание от батареи 3.7V Li-Po или Li-Ion
  RST-Перезагрузка, не используется
  NET-Антенна, ничего не подключаем кроме антенны, а лучше вообще не трогать

  Нужно использовать конвертер для сдвига логических уровней на 3.3V!
  ---------------------------------------------------------------------
  BMP280:
  VCC-3.3V
  GND-GND
  SCL-A5
  SCA-A4
  Не забудьте указать адрес: iarduino_Pressure_BMP sensor(0x77);
  По умолчанию 0x77
  ---------------------------------------------------------------------
*/
#include <iarduino_Pressure_BMP.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <SD.h>
iarduino_Pressure_BMP sensor(0x76);
SoftwareSerial gpsSerial(2, 8);
TinyGPS gps;
File GPS_file;

bool newdata = false;
unsigned long start;
long lat, lon;
unsigned long time, date;
int com = 0;

void setup() {
  gpsSerial.begin(9600);
  Serial.begin(9600);
  sensor.begin();
  pinMode(10, OUTPUT);

  if (!SD.begin(10)) { //Проверка и инициализация sd карты
    Serial.println("SD-");
    return;
  }
  Serial.println("SD+");
  GPS_file = SD.open("GPSLOG.txt", FILE_WRITE);
  if (GPS_file) {
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
    GPS_file.print("PRESSURE");
    GPS_file.print("ALTITUDE (BMP280)");
    GPS_file.println();
    GPS_file.close();
    Serial.println("done.");
  } else {
    Serial.println("error opening test.txt");
  }
  Serial.print("ALT: "); //Потом уберу
  Serial.print("SPD: ");
  Serial.print("LAT: ");
  Serial.print("LON: ");
  Serial.println();
}

void loop() {
 
  if (sensor.read(2)){
   // Serial.println((String)sensor.pressure","sensor.altitude + " M."); // Читаем показания (давление возвращается в Па)
  }
  else                               {
    Serial.println("BMP280 Fail");
  }

  if (millis() - start > 1000) {
    newdata = readgps();
    if (newdata) {
      start = millis();
      gps.get_position(&lat, &lon);
      gps.get_datetime(&date, &time);

      Serial.print ("Date ");  //Вывод значений (потом уберу)
      Serial.print(date);
      Serial.println();
      Serial.print ("Time ");
      Serial.print(time * 0.000001 + 5);
      Serial.print(lat);
      Serial.print("Lat ");
      Serial.println();
      Serial.print(lon);
      Serial.println();
      Serial.println();
      Serial.print(gps.f_speed_kmph());
      Serial.print(gps.f_altitude());
    }
  }
  GPS_file = SD.open("GPSLOG.txt", FILE_WRITE);  //Запись значений на sd
  if (GPS_file) {
    GPS_file.print(lat);
    GPS_file.print(",");
    GPS_file.print(lon);
    GPS_file.print(",");
    GPS_file.print(date);
    GPS_file.print(",");
    GPS_file.print(time * 0.000001 + 5);
    GPS_file.print(",");
    GPS_file.print(gps.f_altitude());
    GPS_file.print(sensor.pressure);
    GPS_file.print(sensor.altitude);
    GPS_file.println();
    GPS_file.close();
  } else {
    Serial.println("SD-");
  }
}
bool readgps() {
  while (gpsSerial.available()) {
    int b = gpsSerial.read();
    if ('\r' != b) {  //Ошибка в библиотеке, игнорим ее
      if (gps.encode(b))
        return true;
    }
  }
  return false;
}
