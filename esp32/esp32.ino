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
#include <ArduinoMqttClient.h>
#include "connection.h"

WiFiClient espClient;
MqttClient mqttClient(espClient);

const char broker[] = "193.147.79.118";
int port = 21883;
const char topic[]  = "/SETR/2024/9/";

String teamName = "murcianosfuriosos";
String teamID = "9";

void advice_arduino(String msg) {
  while (1) {
    if (Serial2.available() != 0) {
      while (Serial2.available() > 0) {
        Serial2.read();
      }
      break;
    } else {
      Serial2.print(msg);
    }
  }
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(sshouse, password);
  // WiFi.begin(ssid, WPA2_AUTH_PEAP, EAP_IDENTITY, EAP_USERNAME, EAP_PASSWORD);
  // Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }

  // Wait the ACK of the ARDUINO, while we send the connect confirmation
  advice_arduino("{ 'wifi': " + String(1) + " }");
}

void initMQTT() {
  while (1) {
    if (!mqttClient.connect(broker, port)) {
      Serial.print('.');
      delay(1000);
    }
    else
      break;
  }
  advice_arduino("{ 'mqtt': " + String(1) + " }");
}

String get_json(String action, int time, int dist, float value) {
  if (time == -1 && dist == -1 && value == -1.00) { // The json msg contain just the principal info
    return "{ \"team_name\" : " + String(teamName) + ", \"id\" : " + String(teamID) + ", \"action\" : " + action + "}";

  } else if (time != -1 && dist == -1 && value == -1.00) { // The json msg contain just the time without the dist
    return "{ \"team_name\" : " + String(teamName) + ", \"id\" : " + String(teamID) + ", \"action\" : " + action + ", \"time\" : " + String(time) + "}";

  } else if (time == -1 && dist != -1 && value == -1.00) { // The json msg contain just the dist without the time
    return "{ \"team_name\" : " + String(teamName) + ", \"id\" : " + String(teamID) + ", \"action\" : " + action + ", \"distance\" : " + String(dist) + "}";

  } else if (time == -1 && dist == -1 && value != -1.00) { // The json msg contain just the value without the time or the dist
    return "{ \"team_name\" : " + String(teamName) + ", \"id\" : " + String(teamID) + ", \"action\" : " + action + ", \"value\" : " + String(value) + "}";

  } else {
    return "{ \"team_name\" : " + String(teamName) + ", \"id\" : " + String(teamID) + "}";
  }
}

String extractValue(String input, String key) {
    input.replace("'", "\"");
    key = "\"" + key + "\":";

    int keyIdx = input.indexOf(key);
    if (keyIdx == -1) {
        return "";
    }
    keyIdx += key.length();

    int valueEndIdx = input.indexOf(',', keyIdx);
    if (valueEndIdx == -1) {
        valueEndIdx = input.indexOf('}', keyIdx);
    }
    if (valueEndIdx == -1) {
        return "";
    }

    String value = input.substring(keyIdx, valueEndIdx);
    value.trim();
    return value;
}



void setup() {
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  initWiFi();
  initMQTT();
  mqttClient.beginMessage(topic);
  mqttClient.print(get_json("START_LAP", -1, -1, -1.00));
  mqttClient.endMessage();
}

String recvBuff = "";

void loop() {
  mqttClient.poll();

  if (Serial2.available() > 0) {
    char c = Serial2.read();
    recvBuff += c;
    if (c == '}')  {
      String endlValue = extractValue(recvBuff, "endl");
      if (endlValue != "") {
        mqttClient.beginMessage(topic);
        mqttClient.print(get_json("END_LAP", endlValue.toInt(), -1, -1.00));
        mqttClient.endMessage();
      }

      String pingValue = extractValue(recvBuff, "ping");
      if (pingValue != "") {
        mqttClient.beginMessage(topic);
        mqttClient.print(get_json("PING", pingValue.toInt(), -1, -1.00));
        mqttClient.endMessage();
      }

      String strlValue = extractValue(recvBuff, "strl");
      if (strlValue != "") {
        mqttClient.beginMessage(topic);
        mqttClient.print(get_json("START_LAP", -1, -1, -1.00));
        mqttClient.endMessage();
      }

      String lineValue = extractValue(recvBuff, "line");
      if (lineValue != "") {
        mqttClient.beginMessage(topic);
        mqttClient.print(get_json("LINE_LOST", -1, -1, -1.00));
        mqttClient.endMessage();
      }

      String obsValue = extractValue(recvBuff, "obst");
      if (obsValue != "") {
        mqttClient.beginMessage(topic);
        mqttClient.print(get_json("OBSTACLE_DETECTED", -1, obsValue.toInt(), -1.00));
        mqttClient.endMessage();
      }
      recvBuff = "";
    }
  }
}

