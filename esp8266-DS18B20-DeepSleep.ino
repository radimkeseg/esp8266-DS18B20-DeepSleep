/*
 * low consumption temp/humidity sensor
 * inspiration
 * https://openhomeautomation.net/esp8266-battery/
 */
 
// Library
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <OneWire.h> 
#include <DallasTemperature.h>
#include "Interval.h"
#include <ThingSpeak.h>

#define TEMP_BUS 2       //(GPIO2)

OneWire oneWire(TEMP_BUS); 
DallasTemperature sensors(&oneWire);

// www.ThingSpeak.com
unsigned long myChannelNumber = 999999 //!!! set your channel number !!!
const char * myWriteAPIKey = "XXXXXXXXXXXXXXXX"; //!!! set you write api key !!!

// WiFi settings
const char* ssid = "SSID";
const char* password = "password";

// Time to sleep (in seconds):
const int sleepTimeS = 10*60;

void write_ThingsSpeak(){

  float temperature; // raw values from sensors
  temperature = read_temp();

  if (isnan(temperature))
  {
    Serial.println("Failed to read from DS18B20 sensor!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  //send data to thingSpeak
  ThingSpeak.setField(3,temperature); //!!!! change field number !!!
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);        
}

float  read_temp(){
 sensors.requestTemperatures(); // Send the command to get temperature readings 
 Serial.print("Temperature: "); 
 float value_temp=sensors.getTempCByIndex(0); // Why "byIndex"?
 Serial.print(value_temp);   
 Serial.println("C");   

 return value_temp;
}

void setup() 
{

  // Serial
  Serial.begin(9600);
  Serial.println("ESP8266 in normal mode");
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Print the IP address
  Serial.println(WiFi.localIP()); 

  sensors.begin();
}

void loop() 
{
  // Use WiFiClient class to create TCP connections
  WiFiClient wfclient;

  //setup thingSpeak
  Serial.println("start sending data to ThingSpeak.com");
  ThingSpeak.begin(wfclient);
  write_ThingsSpeak();
  Serial.println("end sending data to ThingSpeak.com");

  Serial.println();
  Serial.println("closing connection");

  // Sleep
  Serial.println("ESP8266 in sleep mode");
  ESP.deepSleep(sleepTimeS * 1000000);
}
