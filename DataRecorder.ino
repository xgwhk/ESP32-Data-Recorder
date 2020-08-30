/*
 * References
 * NTP:          https://randomnerdtutorials.com/esp32-date-time-ntp-client-server-arduino/
 * DS18B20:      https://randomnerdtutorials.com/esp32-multiple-ds18b20-temperature-sensors/
 * BME280:       https://randomnerdtutorials.com/bme280-sensor-arduino-pressure-temperature-humidity/
 * ESP32 pinout: https://circuits4you.com/2018/12/31/esp32-devkit-esp32-wroom-gpio-pinout/
 */
// Librairy for WiFi connection
#include <WiFi.h>

// Librairy for the NTP server and time management
#include <time.h>

//Librairy for the SDcard reader
#include <SPI.h>
#include <SD.h>
#include <FS.h>

// Librairies and Communication setup for BME280
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
// - Selecting the I2C interface for communication with the BME280
Adafruit_BME280 bme;

// Librairies and Communication setup for the Dallas Temperature Sensor
#include <OneWire.h>
#include <DallasTemperature.h>
// - Data wire is connected on gpio15
#define ONE_WIRE_BUS 15
OneWire oneW(ONE_WIRE_BUS);
// - Pass the oneW reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneW);

// Variables
// - Wifi
const char* ssid = "Provence";
const char* password = "r3LvxuJHZW";
// - BME Sensor
float bmeT = 0;
float bmeH = 0;
int bmeP = 0;
// - NTP server
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 28800;
const int   daylightOffset_sec = 0;
// - SDcard
const int chipSelect = 2;
String CoreFileName = "QC_";
String FileName = "";  
String entetecsv = "time,bme-t,bme-h,bme-p,ds1,ds2,ds3";
bool FileFound = 1;
int Index = 1;
File dataFile;

void setup()
{
  pinMode(2, OUTPUT);
  Serial.begin(115200);
  delay(10);
  SD.begin(chipSelect);
  delay(10);
  ConnectWifi();
  TestingBME();
  TestingDS();
  TestingSDcard();  
}

void loop()
{

}

// Connecting to the Wifi and set time
void ConnectWifi()
{
  Serial.println("");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected :)");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  else {
    Serial.print("Date: ");
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  }
}

// Testing the presence of the BME280 Sensor set as default
void TestingBME()
{
  bool status;
  status = bme.begin(0x76);
  Serial.println("");
  if (!status) {
    Serial.println("Could not find the BME280 sensor, check your wiring !");
  }
  else {
    Serial.print("BME280 data: ");
    GetBME();
    Serial.print("T:");
    Serial.print(bmeT);
    Serial.print("C - ");
    Serial.print("H:");
    Serial.print(bmeH);
    Serial.print("% - ");
    Serial.print("P:");
    Serial.print(bmeP / 100);
    Serial.println("hPa");
  }  
}

// Testing the presence of any Dallas Sensors and showing their ID
void TestingDS()
{
  byte i;
  byte addr[8];
  Serial.println("");
  if (!oneW.search(addr)) {
    Serial.println(" End of DS18B20 search.");
    oneW.reset_search();
    delay(250);
    return;
    Serial.print(" ROM=");
    for (i = 0; i < 8; i++) {
      Serial.write(' ');
      Serial.print(addr[i], HEX);
    }
  }
}

// Testing the presence of the SDcard reader, its type and space left on the card
void TestingSDcard()
{
  Serial.println("");
  if(!SD.begin(chipSelect)) {
    Serial.println("SD-card mount failed!");
    return;
  }
// Check the type of the loaded SD-card
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }
  Serial.print("SD-card Type: ");
  if(cardType == CARD_MMC){ Serial.println("MMC"); }
  else if(cardType == CARD_SD){ Serial.println("SDSC"); }
  else if(cardType == CARD_SDHC){ Serial.println("SDHC"); }
  else { Serial.println("UNKNOWN"); }
  Serial.printf("Space used  : %lluMB\n", SD.usedBytes() / (1024 * 1024));
  
// Generate the filename and test if exist, will increment the index number until it cannot find it
  FileName = "/" + CoreFileName+String(Index) + ".csv";
  while (SD.exists(FileName)) {
    Serial.print(Index);
    Serial.print(", ");
    Index++;
    FileName= "/" + CoreFileName+String(Index) + ".csv";
    }

// Create the filename with the correct new index value and add the header into the newly created CSV file
  dataFile = SD.open(FileName, FILE_WRITE);
  dataFile.println(entetecsv);
  dataFile.close();
  Serial.print("File written and close: ");
  Serial.println(FileName);
}

// Get the data from  the BME280 sensor
void GetBME()
{
  bmeT = bme.readTemperature();
  bmeH = bme.readHumidity();
  bmeP = bme.readPressure();
}
