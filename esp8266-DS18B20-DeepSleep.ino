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

#include "MyPubSub.h"
MyPubSub *myPubSub;

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

#define SETUP_PIN 0      //GPIO0
#define TX 1             //LED_BUILTIN
#define RX 3
/* //node mcu test and debug
#define SETUP_PIN D7      //GPIO0
#define TX LED_BUILTIN
#define RX D7
*/

#define AP_NAME "DigiTemp-v4"

void setup() 
{
  // Serial
  Serial.begin(115200);
  Serial.println("MAC: " + myWifi.getMAC());

  pinMode(SETUP_PIN, OUTPUT); 
  digitalWrite(SETUP_PIN,HIGH);
  delay(500);
  pinMode(SETUP_PIN, INPUT); 
  pinMode(TX, OUTPUT);
  WiFi.hostname(AP_NAME);
  digitalWrite(TX,LOW);
  for(int i=0; i<6; i++){//blink 3 times
    if ( digitalRead(SETUP_PIN) == LOW ) { //when 0/LOW go to wifi setup mode
      isInSetupMode = true;
      digitalWrite(TX,LOW);
      Serial.println("manual config portal triggered");
      myWifi.forceManualConfig((String(AP_NAME)+"-"+String(ESP.getChipId(), HEX)+"-Config").c_str());
      //after the force Manual Config the ESP restarts
    }
    digitalWrite(TX,i%2);
    delay(500);
  }
  digitalWrite(TX,HIGH);
  
  myDallas.begin();

  myWifi.setup(AP_NAME,60); //1 min to configure the WIFI 
  myWifi.setDataHandler( stFncHandleData );

  myThingSpeak.begin(myWifi.getWifiClient());
  myThingSpeak.setup(myWifi.getCustomSettings().settings.TS_CHANNEL, myWifi.getCustomSettings().settings.TS_API_WRITE, myWifi.getCustomSettings().settings.TS_FIELD_TEMP, myWifi.getCustomSettings().settings.GS_UPDATE_INTERVAL);
  
  myWifi.getCustomSettings().print();

  update_interval = ((myWifi.getCustomSettings().settings.GS_UPDATE_INTERVAL<=0)?(600):(myWifi.getCustomSettings().settings.GS_UPDATE_INTERVAL)); //default 10 min


  //for now hardcoded - need to add settings to have it customizable - similar to thingspeak settings
  myPubSub = new MyPubSub(myWifi.getWifiClient(), myWifi.getCustomSettings().settings.MQTT_BROKER, myWifi.getCustomSettings().settings.MQTT_IN_TOPIC, myWifi.getCustomSettings().settings.MQTT_OUT_TOPIC );
  myPubSub->setCredentials(myWifi.getCustomSettings().settings.MQTT_DEVICE_ID, myWifi.getCustomSettings().settings.MQTT_USER, myWifi.getCustomSettings().settings.MQTT_PASSWORD);
  myPubSub->setup();

  
  delay(500);
  
  pinMode(SETUP_PIN, OUTPUT); 
  digitalWrite(SETUP_PIN,HIGH);
  delay(500);
  pinMode(TX, OUTPUT);
  pinMode(SETUP_PIN, INPUT); 

  for(int i=0;i<30; i++){ //blink 15 times
    if ( digitalRead(SETUP_PIN) == LOW ) { //when 0/LOW go to no sleep mode
      isInSetupMode = true;
      digitalWrite(TX,LOW);
      Serial.println("no sleep mode triggered");
      break;
    }
    digitalWrite(TX,i%2);
    delay(100);
  }

  if(!isInSetupMode) digitalWrite(TX,HIGH); //keep the blue if no sleep mode
}

void loop() 
{
  // Handle web server
  myWifi.handleClient();
  myPubSub->handleClient();
  
  if(measurementUpdate.expired()){ //read if the update interval has expired only
    measurementUpdate.set(update_interval*1000); // set new interval period 

    Serial.println("Reading sensors:");
    myDallas.measure();
        
    if(myWifi.getCustomSettings().settings.THINGSPEAK){ //write to thingspeak
      if(myWifi.isConnected()){
        myThingSpeak.write(myDallas.getLastMeasured());
      }
    }

    if(myWifi.getCustomSettings().settings.MQTT){ //write to mqtt
      myPubSub->reconnect();
      myPubSub->publish(stFncHandleData().c_str(),false);
    }
  }
  
  yield();
  delay(500); //just to give some time, reaction to http requests might be a bit delayed, but OK


  if(!isInSetupMode && myWifi.getCustomSettings().settings.GS_DEEP_SLEEP){ //if not in setup mode then measure and deep sleep (if deep sleep enabled)
    // Sleep
    Serial.println("ESP8266 in sleep mode");
    ESP.deepSleep(update_interval * 1000000);
  }
  
}
