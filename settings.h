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

#pragma once
#include <FS.h> 

#define update_username "admin"
#define update_password "admin"

// TimeClient settings
typedef struct
{
  boolean GS; //mesure periodically and send 
  unsigned int GS_UPDATE_INTERVAL;
  boolean GS_DEEP_SLEEP;  

  boolean THINGSPEAK;
  unsigned long TS_CHANNEL;
  char TS_API_WRITE[17];
  unsigned int TS_FIELD_TEMP;

  boolean MQTT;
  char MQTT_BROKER[256];
  char MQTT_USER[256];
  char MQTT_PASSWORD[256];
  char MQTT_DEVICE_ID[256];
  char MQTT_OUT_TOPIC[256];
  char MQTT_IN_TOPIC[256];
  
} settings_t;


/***************************
 * End Settings
 **************************/

class CustomSettings{
  private:
     const String CUSTOM_SETTINGS = "/settings/custom.txt";
  
  public:
    settings_t settings;

    CustomSettings(){
      settings.GS = false;
      settings.GS_DEEP_SLEEP=false;
      settings.GS_UPDATE_INTERVAL=10*60;
      settings.THINGSPEAK = false;
      settings.MQTT = false;
    }
    void init();

    void write();
    void read();
    void print();
}; 
