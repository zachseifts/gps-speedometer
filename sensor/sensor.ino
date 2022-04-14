#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <DHT.h>

/* A simple gps interface for the arduino to communicate over a Serial port.
 * 
 * This sketch uses the TinyGPSPlus library to write data to the
 * main serial port on an Arduino in a python friendly format.
 * 
 * This is based on the example that come with the TinyGPSPlus library.
 * 
 * It returns a csv of the sensor's following readings:
 * 
 *  - number of satellites visable
 *  - latitude
 *  - longitude
 *  - altitude in feet
 *  - speed in mph
 *  - course in deg
 *  - datetime in ISO format
 *  - temperature in F
 *  - humidity
 */

// Variables for sensor pins.
static const int DHTPin = 2;
static const int TXPin = 3;
static const int RXPin = 4;

// Variables for the GPS board.
static const uint32_t GPSBaud = 9600;
static const uint32_t SerialBaud = 115200;

// Variables for the DHT sensor.
static const int maxReadings = 60;

// Variables to cache DHT sensor data.
int readings = 0;
float temp = 0;
float humidity = 0;

// Create the TinyGPSPlus object.
TinyGPSPlus gps;

// GPS Serial port.
SoftwareSerial ss(RXPin, TXPin);

// Initalize the DHT sensor
DHT dht(DHTPin, DHT11);

void setup()
{
  // Connect to the internal serial port.
  Serial.begin(SerialBaud);

  // Connect to the gps board's serial port.
  ss.begin(GPSBaud);

  // Connect to the DHT11 sensor.
  dht.begin();
}

void loop()
{
  readDHTData();
  writeSerialData();
}

// Only grab sensor data every 20 cycles (seconds right now).
static void readDHTData() {
  if (readings >= maxReadings) {
    temp = dht.readTemperature();
    humidity = dht.readHumidity();
    readings = 0;
  }
  readings++;
}

// Write data out to the serial port.
static void writeSerialData() {

  // Formats GPS data.
  Serial.print(gps.satellites.value());
  Serial.print(F(","));
  Serial.print(gps.location.lat(), 6);
  Serial.print(F(","));
  Serial.print(gps.location.lng(), 6);
  Serial.print(F(","));
  Serial.print(gps.altitude.feet(), 2);
  Serial.print(F(","));
  Serial.print(gps.speed.mph());
  Serial.print(F(","));
  Serial.print(gps.course.deg());
  Serial.print(F(","));

  // Formats the time and date into python readable format.
  char d[32];
  sprintf(d, "%02d-%02d-%02d ", gps.date.year(), gps.date.month(), gps.date.day());
  Serial.print(d);
  char t[32];
  sprintf(t, "%02d:%02d:%02d", gps.time.hour(), gps.time.minute(), gps.time.second());
  Serial.print(t);

  // Formats weather data.
  Serial.print(F(","));
  Serial.print(humidity);
  Serial.print(F(","));
  Serial.print(temp);
  Serial.print(F(","));
  Serial.println();

  smartDelay(1000);
}

// A non-blocking delay function.
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}
