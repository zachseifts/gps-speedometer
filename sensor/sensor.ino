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
 *  - temperature in F
 *  - humidity
 *  - datetime in ISO format
 */

// Setup some variables for pins and baud rates and sensor pins.
static const int RXPin = 13;
static const int TXPin = 12;
static const int DHTPin = 2;

static const uint32_t GPSBaud = 9600;
static const uint32_t SerialBaud = 115200;

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

  dht.begin();
}

void loop()
{
  // Start writing out gps data on the internal Serial port.
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
  printDateTime(gps.date, gps.time);
  Serial.print(F(","));
  Serial.print(dht.readHumidity());
  Serial.print(F(","));
  Serial.print(dht.readTemperature());
  Serial.print(F(","));
  Serial.println();

  smartDelay(100);

  // Check to see if the board is properly configured.
  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received,"));
}

static void readGPSData() {
  
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

// Formats the time for python's datetime.fromisoformat().
static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
  if (!d.isValid())
  {
    Serial.print(F(""));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d-%02d-%02d ", d.year(), d.month(), d.day());
    Serial.print(sz);
  }
  
  if (!t.isValid())
  {
    Serial.print(F(""));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d", t.hour(), t.minute(), t.second());
    Serial.print(sz);
  }

  smartDelay(0);
}
