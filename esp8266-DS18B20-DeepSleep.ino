/*
 * low consumption temp/humidity sensor
 * inspiration
 * https://openhomeautomation.net/esp8266-battery/
 */

 
// Library
#include "MyDallas.h"
MyDallas myDallas;

#include "MyThingSpeak.h"
MyThingSpeak myThingSpeak;

#include "MyWifi.h"
MyWifi myWifi; 

static String stFncHandleData(){
  String res;
  res = "{\"temp\":\"";
  res += myDallas.getLastMeasured();
  res += "\"}";
  return res;
}

Interval measurementUpdate;
long update_interval = 600*1000; //default 10 min
bool isInSetupMode = false;

#define SETUP_PIN 0      //(GPIO0)
#define TX 1
#define RX 3

#define AP_NAME "DigiTemp-v3"

void blink(signed int count){
    for(int x=0; x<count; x++){
     digitalWrite(TX,HIGH);
     delay(500); 
     digitalWrite(TX,LOW);
     delay(500); 
    }  
}

void setup() 
{

  // Serial
  Serial.begin(9600);
  WiFi.hostname(AP_NAME);

  delay(500);
  pinMode(SETUP_PIN, INPUT); 

  for(int i=0; i<100; i++){
    if ( digitalRead(SETUP_PIN) == LOW ) { //when 0/LOW go to setup mode - no deep sleep
      isInSetupMode = true;
      blink(3);
      Serial.println("manual config portal triggered");
      myWifi.forceManualConfig((String(AP_NAME)+"-"+String(ESP.getChipId(), HEX)+"-Config").c_str());
      Serial.println("connecting again ...)");
    }
    delay(50);
  }

  myDallas.begin();

  myWifi.setup(AP_NAME,60); //1 min to configure the WIFI 
  myWifi.setDataHandler( stFncHandleData );

  myThingSpeak.begin(myWifi.getWifiClient());
  myThingSpeak.setup(myWifi.getCustomSettings().settings.TS_CHANNEL, myWifi.getCustomSettings().settings.TS_API_WRITE, myWifi.getCustomSettings().settings.TS_FIELD_TEMP, myWifi.getCustomSettings().settings.TS_UPDATE_INTERVAL);
  
  myWifi.getCustomSettings().print();

  update_interval = ((myWifi.getCustomSettings().settings.TS_UPDATE_INTERVAL<=0)?(600):(myWifi.getCustomSettings().settings.TS_UPDATE_INTERVAL)); //default 10 min
  
  delay(1000);
}

void loop() 
{
  // Handle web server
  myWifi.handleClient();
  
  if(measurementUpdate.expired()){ //read if the update interval has expired only
    measurementUpdate.set(update_interval*1000); // set new interval period 

    Serial.println("Reading sensors:");
    myDallas.measure();
        
    if(myWifi.getCustomSettings().settings.THINGSPEAK){ //write to thingspeak
      if(myWifi.isConnected()){
        myThingSpeak.write(myDallas.getLastMeasured());
      }
    }
  
  }
  
  delay(1000); //just to give some time, reaction to http requests might be a bit delayed, but OK

  if(!isInSetupMode){ //if not in setup mode then measure and deep sleep
    // Sleep
    Serial.println("ESP8266 in sleep mode");
    ESP.deepSleep(update_interval * 1000000);
  }
}
