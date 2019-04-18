/*
 * Project:     mhip_tracker
 * Description: GPS tracker using 2G/3G communication.
 * Author:      Timothy Fleming, MagicHome Inc
 * Date:        2019-03-24
 * Copyright:   MIT (see: https://opensource.org/licenses/MIT)
 */

#include <Adafruit_GPS.h>
#include <Adafruit_DHT.h>

#define DHTPIN 3     // Digital pin connected to the DHT sensor 

// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview
DHT dht(DHTPIN, DHTTYPE);

// what's the name of the hardware serial port?
#define GPSSerial Serial1

// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);

// these are the intervals for when events get sent
// GPS cloud interval varies depending on speed; faster -> more frequent updates
uint32_t TI_GPS_CLOUD_MIN = 20 * 60 * 1000;   // 20 min, interval when we're stationary
uint32_t TI_GPS_CLOUD_MAX =  1 * 60 * 1000;   //  1 min, interval when we're moving fast
uint32_t TI_DHT_CLOUD     = 10 * 60 * 1000;   // 10 min
// MESH updates are all the same.  Sensors are read at this rate.
uint32_t TI_ALL_MESH      = 2000;             //  2 sec

// these are appended to the device name to create event names
#define EVENT_NAME_LOCATION "gps"
#define EVENT_NAME_TEMP "dht"

// init timer counts
uint32_t timer_gps_cloud = TI_GPS_CLOUD_MIN;
uint32_t timer_dht_cloud = TI_DHT_CLOUD;
uint32_t timer_mesh = 1000;

// these are used to apply some quality control to temp values
float t_last;
uint16_t t_count;

// message buffers
char msg_gps [250];
char msg_dht [120];

// ============================================================================
// GPS : Location Functions 
// ============================================================================

// Configure GPS board
void setupGPS() {
  Serial.println("GPS : setup");

  // This defines a variable with the particle cloud.  It enables us to query the value any time.
  Particle.variable("gps", msg_gps, STRING);

  // Note: the following code/comments are pasted from the Adafruit example code.  I don't actually know this
  // much about the GPS module.

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);

  // Ask for firmware version
  GPSSerial.println(PMTK_Q_RELEASE);
}

// GPS processing in main loop
void loopGPS() {
  // location formatting
  int32_t lat_units = GPS.latitude_fixed / 10000000;
  int32_t lat_decimal = GPS.latitude_fixed - (lat_units * 10000000);

  int32_t lon_units = GPS.longitude_fixed / 10000000;
  int32_t lon_decimal = GPS.longitude_fixed - (lon_units * 10000000);

  // unit conversions
  float speed_mph = GPS.speed * 1.15078;
  float alt_ft = GPS.altitude * 3.28084;

  // Format GPS message as json
  // Example: {"gps_timestamp":"19-03-26T20:04:33","latitude":" 39.7766033N","longitude":"105.1291466W","altitude": 5416.01,"speed":  0.17,"heading":257.14,"satellites":11,"quality":2}
  snprintf(msg_gps, 250, "{\"gps_timestamp\":\"%u-%02u-%02uT%02u:%02u:%02u\",\"latitude\":\"%3lu.%7lu%c\",\"longitude\":\"%3lu.%7lu%c\",\"altitude\":%8.2f,\"speed\":%6.2f,\"heading\":%6.2f,\"satellites\":%u,\"quality\":%u}", \
    GPS.year, GPS.month, GPS.day, GPS.hour, GPS.minute, GPS.seconds, \
    lat_units, lat_decimal, GPS.lat, \
    lon_units, lon_decimal, GPS.lon, \
    alt_ft, speed_mph, GPS.angle, GPS.satellites, GPS.fixquality);

  // output to console
  Serial.print("GPS: "); Serial.print(msg_gps);

  // publish to mesh network
  Mesh.publish(EVENT_NAME_LOCATION, msg_gps);

  // publish to particle.io cloud
  uint32_t dynamic_timer_interval = calcCloudUpdateInterval(speed_mph);
  if (millis() - timer_gps_cloud > dynamic_timer_interval) {
    timer_gps_cloud = millis(); // reset the timer
    Serial.print("*"); // give indication on console value that this one was published to cloud
    Particle.publish(EVENT_NAME_LOCATION, msg_gps, PRIVATE);
  }
  Serial.println("");
}

// The cloud update interval varies depending on speed.  Calculate the desired interval given the current speed.
int32_t calcCloudUpdateInterval(int32_t speed) 
{
  int32_t result = TI_GPS_CLOUD_MIN;

  if ( speed > 60 )
  {
    result = TI_GPS_CLOUD_MAX;
  } 
  else if ( speed > 2 )
  {
    result = (60 / speed) * TI_GPS_CLOUD_MAX;
  }

  return result;
}


// ============================================================================
// DHT : Temperature / Humidity Functions 
// ============================================================================

// Configure DHT temp/humidity sensor
void setupDHT() {
  // Initialize device.
  Serial.println("DHT : setup");

  // This defines a variable with the particle cloud.  It enables us to query the value any time.
  Particle.variable("dht", msg_dht, STRING);

  dht.begin();
}

// Temp/humidity sensor processing in main loop.
void loopDHT() {
  // read value from sensor
  float t = dht.getTempFarenheit();   // temp, F
  float h = dht.getHumidity();        // humidity, %
  
  // Check if any reads failed and exit early (to try again).
  if ( !validateTempurature(t,h) ) {
    return;
  }

  // these are read as degrees C, convert to F.
  float hi = dht.getHeatIndex() * 1.8 + 32; // heat index, F
  float dp = dht.getDewPoint() * 1.8 + 32;  // dew point, F

  // format temp/humidity message as json
  // Example: {"temperature": 70.16,"humidity": 24.00,"dew_point": 31.68,"heat_index": 75.12}
  snprintf(msg_dht, 120, "{\"temperature\":%6.2f,\"humidity\":%6.2f,\"dew_point\":%6.2f,\"heat_index\":%6.2f}", t, h, dp, hi);

  // output to serial console
  Serial.print("DHT: "); Serial.println(msg_dht);
 
  // publish to mesh network
  Mesh.publish(EVENT_NAME_TEMP, msg_dht);
  
  // publish to particle.io cloud
  if (millis() - timer_dht_cloud > TI_DHT_CLOUD) {
    // reset timer
    timer_dht_cloud = millis(); // reset the timer
    // publish dht data
    Particle.publish(EVENT_NAME_TEMP, msg_dht, PRIVATE);
  }
}

// Sometimes the DHT will output wildly inaccurate temperatures.  If the current temp differs 
// by over 25% from the last value, we'll consider it invalid.  We'll retry the temp '5' times 
// before reporting the (possibly) invalid temp.  Because, after all, the Tracker  could be 
// on fire and the temp is valid.
boolean validateTempurature(float t, float h) {
  // the two checks below are never valid so we count the retrys
  if (isnan(h) || isnan(t)) {
    Serial.println("DHT : failed to read");
    return false;
  }

  if ( t > 200 || t < -200 ) {
    Serial.println("DHT : invalid temperature");
    return false;
  }

  // if we still have reties left to give, and the temp is out of range by some 25%
  // do a retry
  if ( t_count < 5 && (t < t_last * 0.75 || t > t_last * 1.25) ) {
    t_count = t_count + 1;
    return false;
  }

  // valid temp, reset
  t_count = 0;
  t_last = t;

  return true;
}


// ============================================================================
// Setup
// ============================================================================

// setup() runs once, when the device is first turned on.
void setup() {
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);
  Serial.println("MIot Tracker");

  setupGPS();
  setupDHT();
}


// ============================================================================
// Loop
// ============================================================================

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // fix timer wrap arounds
  if (timer_gps_cloud > millis()) timer_gps_cloud = millis();
  if (timer_dht_cloud > millis()) timer_dht_cloud = millis();
  if (timer_mesh > millis()) timer_mesh = millis();

  // read location
  // NB: while it would make more sense to do this in the loopGPS() function, it doesn't work there.
  //  Apparently the GPS.read() function needs to happen very often to get valid data.  
  //  It may have something to do with the "not listening" warning below. Not sure.
  GPS.read();

  bool validGPS = true;

  // if a sentence is received, we can check the checksum, parse it...
  if ( GPS.newNMEAreceived() ) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    if ( !GPS.parse(GPS.lastNMEA()) ) {
      // if we fail to parse a sentence we will wait for another
      // NB: I wouldn't print this to the console because it happens A LOT.
      validGPS = false; 
    }

    // if no fix, don't process
    if ( !GPS.fix ) {
      Serial.println("GPS : no fix");
      validGPS = false;
    }
  }

  if (millis() - timer_mesh > TI_ALL_MESH) {
    // process sensor data
    if ( validGPS ) {
      loopGPS();
    }
    loopDHT();
  }
}
