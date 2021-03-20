/**The MIT License (MIT)

Copyright (c) 2021 by Radim Keseg

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

#include <ESP8266WiFi.h>
#include <PubSubClient.h>


class MyPubSub{
private:
  WiFiClient *wfclient;
  PubSubClient *psclient;
  const char* mqtt_server = "";
  const char* inTopic = "";
  const char* outTopic = "";

  const char* clientID = "";
  const char* user = "";
  const char* password = "";

  void callback(char* topic, byte* payload, unsigned int length);
  
public:
  MyPubSub(WiFiClient& wfclient, const char* mqtt_server, const char* inTopic, const char* outTopic){
    this->wfclient = &wfclient;
    this->mqtt_server = mqtt_server;
    this->inTopic = inTopic;
    this->outTopic = outTopic;
    psclient = new PubSubClient( wfclient );
    
  }
  ~MyPubSub(){
    if(psclient != NULL) delete psclient;
  }

  void setCredentials(const char* clientID, const char* user, const char* password);

  void reconnect();
  bool setup();
  void handleClient();
  
  bool isConnected();
  bool subscribe();
  bool publish(const char* payload){ return publish(payload, false); };
  bool publish(const char* payload, boolean retained);
};
