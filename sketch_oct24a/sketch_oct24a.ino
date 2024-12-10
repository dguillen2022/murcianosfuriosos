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

#include "FastLED.h"

#define RXD2 33
#define TXD2 4

#define PIN_RBGLED 4
#define NUM_LEDS 1

CRGB leds[NUM_LEDS];

uint32_t Color(uint8_t r, uint8_t g, uint8_t b)
{
  return (((uint32_t)r << 16) | ((uint32_t)g << 8) | b);
}

void initWiFi() {
  String recvBuff;
  while(1) {
    if (Serial.available()) {
      char c = Serial.read();
      recvBuff += c;

      Serial.print(recvBuff + "\n");
      if (c == '}')  {
        if (recvBuff == "{ 'wifi': 1 }") {
          FastLED.showColor(Color(0, 255, 0));

          // Send an ACK to the ESP32
          Serial.print("{ 'ack': " + String(1) + " }");
          break;
        }
        recvBuff = "";
      }
    }
  }
}

void setup() {

  Serial.begin(9600);

  FastLED.addLeds<NEOPIXEL, PIN_RBGLED>(leds, NUM_LEDS);
  FastLED.setBrightness(20);

  // Set Red Color to LED
  FastLED.showColor(Color(255, 0, 0));
  initWiFi();
}

void loop () {
  // Serial.println("ARDUINO LOOP");
}
