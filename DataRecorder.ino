/*
 * References
 * https://randomnerdtutorials.com/esp32-date-time-ntp-client-server-arduino/
 * https://randomnerdtutorials.com/esp32-multiple-ds18b20-temperature-sensors/
 * https://randomnerdtutorials.com/bme280-sensor-arduino-pressure-temperature-humidity/
 */
// Librairy for WiFi connection
#include <WiFi.h>

// Librairy for the NTP server and time management
#include <time.h>

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

void setup()
{
  Serial.begin(115200);
  delay(10);
  ConnectWifi();
  TestingBME();
  TestingDS();
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
  if (!status) {
    Serial.println("");
    Serial.println("Could not find the BME280 sensor, check your wiring !");
  }
  else {
    Serial.println("");
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

// Testing thge presence of any Dallas Sensors and showing their ID
void TestingDS()
{
  byte i;
  byte addr[8];
  if (!oneW.search(addr)) {
    Serial.println("");
    Serial.println(" No more DS18B20.");
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

// Get the data from  the BME280 sensor
void GetBME()
{
  bmeT = bme.readTemperature();
  bmeH = bme.readHumidity();
  bmeP = bme.readPressure();
}
