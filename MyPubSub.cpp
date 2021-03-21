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

#include "MyPubSub.h"

void MyPubSub::callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  //if message recieved just blink
  digitalWrite(BUILTIN_LED, HIGH);
  delay(500);
  digitalWrite(BUILTIN_LED, LOW);
}

void MyPubSub::setCredentials(const char* clientID, const char* user, const char* password){
  this->clientID = clientID;
  this->user = user;
  this->password = password;
}


void MyPubSub::reconnect(){
  // Loop until we're reconnected
  if (!psclient->connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (psclient->connect(clientID, user, password)) { //hardcoded needs to be fixed
      Serial.println("connected");
      subscribe();
    } else {
      Serial.print("failed, rc=");
      Serial.print(psclient->state());
    }
  }
  psclient->loop();
}

bool MyPubSub::setup(){
  if(mqtt_server != NULL ){
    psclient->setServer(mqtt_server, 1883);
    psclient->setCallback( [this](char* topic, byte* payload, unsigned int length) { callback(topic, payload, length); } );
    Serial.print("Connected to MQTT server: ");
    Serial.println(mqtt_server);
    return true;
  }
   
  Serial.println("Not Connected to MQTT server: <unspecified>");    
  return false;
}

bool MyPubSub::isConnected(){
  return psclient->connected();
} 

bool MyPubSub::subscribe(){
  return psclient->subscribe(inTopic);
}

bool MyPubSub::publish(const char* payload, boolean retained) {
    Serial.print("OutTopic: "); Serial.println(outTopic);
    Serial.print("Payload: "); Serial.println(payload);
    return psclient->publish(outTopic, (const uint8_t*) payload, strlen(payload), retained);
}
