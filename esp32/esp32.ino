/*
* Copyright (C) 2022 by Roberto Calvo-Palomino
*
*
*  This programa is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with RTL-Spec.  If not, see <http://www.gnu.org/licenses/>.
*
*   Authors: Roberto Calvo-Palomino <roberto.calvo [at] urjc [dot] es>
*/

// Subject: Sistemas Empotrados y de Tiempo Real
// Universidad Rey Juan Carlos, Spain

#define RXD2 33
#define TXD2 4

#include "WiFi.h"
// #include "PubSubClient.h"
#include "connection.h"

/*
WiFiClient espClient;
PubSubClient mqttClient(espClient);
*/

void initWiFi() {

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, WPA2_AUTH_PEAP, EAP_IDENTITY, EAP_USERNAME, EAP_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }

  // Serial.print("IP Address: ");
  // Serial.println(WiFi.localIP());
  // Serial.print("RRSI: ");
  // Serial.println(WiFi.RSSI());

  // Wait the ACK of the ARDUINO, while we send the connect confirmation
  while (1) {
    if (Serial2.available() != 0) {
      break;
    } else {
      Serial2.print("{ 'wifi': " + String(1) + " }");
    }
  }
}

/*
void connectMQTT() {
  mqttClient.setServer(mqtt_server, mqtt_port);
  while (!mqttClient.connected()) {
    if (mqttClient.connect(client_id, mqtt_user, mqtt_password)) {
      break;
    }
  }
}
*/

void setup() {

  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  initWiFi();

}

void loop() {
  Serial.println("ESP32 LOOP");
}

