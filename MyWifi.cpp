/**The MIT License (MIT)

Copyright (c) 2019 by Radim Keseg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "MyWifi.h"
#include "settings.h"

boolean MyWifi::wifi_apply_hostname_(String hostname) {
  Serial.print("Sett wifi station hostname: ");
  Serial.println(hostname.c_str());  
  bool ret = wifi_station_set_hostname(const_cast<char *>(hostname.c_str()));
  if (!ret) {
    Serial.println("Setting WiFi Hostname failed!");
  }
  return ret;
}



void MyWifi::handle_root()
{
  String content = FPSTR(PAGE_INDEX);      

  if (cs.settings.GS) content.replace("{gs}", "checked='checked'");
  else content.replace("{gs}", "");
  content.replace("{gs_update_interval}", String(cs.settings.GS_UPDATE_INTERVAL).c_str() );
  if (cs.settings.GS_DEEP_SLEEP) content.replace("{gs_deep_sleep}", "checked='checked'");
  else content.replace("{gs_deep_sleep}", "");


  if (cs.settings.THINGSPEAK) content.replace("{ts}", "checked='checked'");
  else content.replace("{ts}", "");
  content.replace("{ts_channel}", String(cs.settings.TS_CHANNEL).c_str() );
  content.replace("{ts_write_api_key}", String(cs.settings.TS_API_WRITE).c_str() );
  content.replace("{ts_field_temp}", String(cs.settings.TS_FIELD_TEMP).c_str() );

  if (cs.settings.MQTT) content.replace("{mqtt}", "checked='checked'");
  else content.replace("{mqtt}", "");
  content.replace("{mqtt_broker}", String(cs.settings.MQTT_BROKER).c_str() );
  content.replace("{mqtt_user}", String(cs.settings.MQTT_USER).c_str() );
  content.replace("{mqtt_password}", String(cs.settings.MQTT_PASSWORD).c_str() );
  content.replace("{mqtt_device_id}", String(cs.settings.MQTT_DEVICE_ID).c_str() );
  content.replace("{mqtt_otopic}", String(cs.settings.MQTT_OUT_TOPIC).c_str() );
  content.replace("{mqtt_itopic}", String(cs.settings.MQTT_IN_TOPIC).c_str() );
  
  server->send(200, "text/html", content);
} 


void MyWifi::handle_store_settings(){
  if(server->arg("_gs")==NULL ){
    Serial.println("setting page refreshed only, no params");      
  }else{
    Serial.println("settings changed");  

    cs.settings.GS = server->arg("_gs").length()>0;       
    cs.settings.GS_UPDATE_INTERVAL = atoi(server->arg("_gs_update_interval").c_str());
    cs.settings.GS_DEEP_SLEEP = server->arg("_gs_deep_sleep").length()>0;       

    cs.settings.THINGSPEAK = server->arg("_ts").length()>0;       
    cs.settings.TS_CHANNEL = atol(server->arg("_ts_channel").c_str());
    strncpy(cs.settings.TS_API_WRITE, server->arg("_ts_write_api_key").c_str(), 17); 
    cs.settings.TS_FIELD_TEMP = atoi(server->arg("_ts_field_temp").c_str());

    cs.settings.MQTT = server->arg("_mqtt").length()>0;       
    strncpy(cs.settings.MQTT_BROKER, server->arg("_mqtt_broker").c_str(), 256); 
    strncpy(cs.settings.MQTT_USER, server->arg("_mqtt_user").c_str(), 256); 
    strncpy(cs.settings.MQTT_PASSWORD, server->arg("_mqtt_password").c_str(), 256); 
    strncpy(cs.settings.MQTT_DEVICE_ID, server->arg("_mqtt_device_id").c_str(), 256); 
    strncpy(cs.settings.MQTT_OUT_TOPIC, server->arg("_mqtt_otopic").c_str(), 256); 
    strncpy(cs.settings.MQTT_IN_TOPIC, server->arg("_mqtt_itopic").c_str(), 256);   
  }
  cs.print();          
  cs.write();
  server->send(200, "text/html", "OK - restart");
  
  restart(1);
} 

void MyWifi::handle_data(){
  if(fDataHandler!=NULL){
      server->send(200, "application/json", fDataHandler());
  }else{
      server->send(200, "application/json", "{}");
  }
}

void MyWifi::setDataHandler(fncHandleData fDataHandler){
  this->fDataHandler = fDataHandler;
}

void MyWifi::restart(unsigned int inSec){
    delay(inSec*1000);
    //reset and try again, or maybe put it to deep sleep
    Serial.print("restart in "); Serial.print(inSec); Serial.println(" sec"); 
    pinMode(0, OUTPUT);   
    digitalWrite(0,HIGH);  //from some reason this has to be set before reset|restart https://github.com/esp8266/Arduino/issues/1017
    ESP.reset();
    delay(5000);  
}

void MyWifi::forceManualConfig(const char* APname){
      if (!wifiManager->startConfigPortal(APname)) {
        Serial.println("failed to connect and hit timeout");
        restart(3);
      }  
}


//---------------------------------------------------------
String MyWifi::getIP(){
  char ipAddr[16]; //aaa.bbb.ccc.ddd
  IPAddress ip = WiFi.localIP();
  sprintf(ipAddr, "%d\.%d\.%d\.%d", ip[0], ip[1], ip[2], ip[3]);
  return String(ipAddr);
/*
  String s="";
  IPAddress ip = WiFi.localIP();
  for (int i=0; i<4; i++)
    s += i  ? "." + String(ip[i]) : String(ip[i]);
  return s;
*/
}
String MyWifi::getMAC(){
  return WiFi.macAddress();
}

bool MyWifi::isConnected(){
  return WiFi.status() == WL_CONNECTED;
}


//---------------------------------------------------------
void MyWifi::setup(const char* APname, int timeout_in_sec){
  // Uncomment for testing wifi manager
  //  wifiManager.resetSettings();
  //or use this for auto generated name ESP + ChipID

  //Manual Wifi
  //WiFi->begin(WIFI_SSID, WIFI_PWD);

  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  wifiManager->setTimeout((timeout_in_sec<0)?180:timeout_in_sec);

  String hostname = ((APname==NULL)?String(HOSTNAME):String(APname)) + String("-") + String(ESP.getChipId(), HEX);
  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  Serial.println("AP: "+hostname);
  if(!wifiManager->autoConnect( hostname.c_str() ) ){
    Serial.println("failed to connect and hit timeout");
    
    Serial.println("switching to no wifi mode");
    //restart(); //no wifi mode - uncomment the restart() if endess retry is needed
  }else{     
      // OTA Setup
      WiFi.hostname(hostname);
      
      if( mdns.begin( (hostname) .c_str(), WiFi.localIP() ) ){
         Serial.print("mDNS: "); Serial.print(hostname); Serial.print(".local = "); Serial.println(WiFi.localIP());
      }else{
         Serial.print("mDNS failed");
      }
      
      httpUpdater->setup(server, "/firmware", update_username, update_password );
    
    
      //user setting handling
      server->on("/", std::bind(&MyWifi::handle_root, this));
      server->on("/setup", std::bind(&MyWifi::handle_store_settings,this)); 
      server->on("/data",  std::bind(&MyWifi::handle_data,this)); 
    
      server->begin(); 
      Serial.println("HTTP server started");
    
      mdns.addService("http", "tcp", 80); 
  }
  
  Serial.println("reading settings ...");
  cs.init();
  cs.read();   
  cs.print();
}

void MyWifi::handleClient(){
  // Handle web server
  server->handleClient(); 
}

WiFiClient& MyWifi::getWifiClient(){
  if(wfclient==NULL) wfclient = new WiFiClient();
  return *wfclient;
}

CustomSettings& MyWifi::getCustomSettings(){
   return cs;
}
