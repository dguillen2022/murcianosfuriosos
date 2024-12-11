#include "thread.h"
#include "FastLED.h"

#define PIN_RBGLED 4
#define NUM_LEDS 1

#define PIN_ITR20001_LEFT   A2
#define PIN_ITR20001_MIDDLE A1
#define PIN_ITR20001_RIGHT  A0

#define TRIG_PIN 13  
#define ECHO_PIN 12  

#define PIN_Motor_STBY 3

#define PIN_Motor_AIN_1 7
#define PIN_Motor_PWMA 5

#define PIN_Motor_BIN_1 8
#define PIN_Motor_PWMB 6

#define KP 0.24 // Last 0.12 0.18 -- Perfect: 0.24
#define KD 0.04 // Last 0.04 -- Perfect: 0.04

#define VEL_BASE 60
#define MAX_VEL 60
#define MIN_VEL 30

CRGB leds[NUM_LEDS];
ThreadController controller = ThreadController();
UltraSound* ultraSound;

int ACK_COUNTER = 1;

int vel = 60;
int last_error = 0;

uint32_t Color(uint8_t r, uint8_t g, uint8_t b)
{
  return (((uint32_t)r << 16) | ((uint32_t)g << 8) | b);
}

void ackWiFi() {
  String recvBuff;
  while(1) {
    if (Serial.available()) {
      char c = Serial.read();
      recvBuff += c;

      if (c == '}')  {
        if (recvBuff == "{ 'wifi': 1 }") {
          FastLED.showColor(Color(0, 255, 0));

          // Send an ACK to the ESP32
          Serial.print("{ 'ack': " + String(ACK_COUNTER) + " }");
          ACK_COUNTER++;
          break;
        }
        recvBuff = "";
      }
    }
  }
}

void ackMQTT() {
  String recvBuff;
  while(1) {
    if (Serial.available()) {
      char c = Serial.read();
      recvBuff += c;

      if (c == '}')  {
        if (recvBuff == "{ 'mqtt': 1 }") {
          FastLED.showColor(Color(0, 255, 0));

          // Send an ACK to the ESP32
          Serial.print("{ 'ack': " + String(ACK_COUNTER) + " }");
          ACK_COUNTER++;
          break;
        }
        recvBuff = "";
      }
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  FastLED.addLeds<NEOPIXEL, PIN_RBGLED>(leds, NUM_LEDS);
  FastLED.setBrightness(20);

  FastLED.showColor(Color(255, 0, 0));
  ackWiFi();
  FastLED.showColor(Color(255, 0, 0));
  ackMQTT();
  FastLED.showColor(Color(0, 255, 0));

  ultraSound = new UltraSound(TRIG_PIN, ECHO_PIN);
  ultraSound->setInterval(100);

  controller.add(ultraSound);

  pinMode(PIN_Motor_AIN_1, OUTPUT);
  pinMode(PIN_Motor_BIN_1, OUTPUT);

  pinMode(PIN_Motor_PWMA, OUTPUT);
  pinMode(PIN_Motor_PWMB, OUTPUT);

  pinMode(PIN_Motor_STBY, OUTPUT);
  digitalWrite(PIN_Motor_STBY, HIGH);
  digitalWrite(PIN_Motor_AIN_1, HIGH);
  digitalWrite(PIN_Motor_BIN_1, HIGH);
}

void loop() {
  controller.run();
  // put your main code here, to run repeatedly:
  int left = analogRead(PIN_ITR20001_LEFT);
  int mid = analogRead(PIN_ITR20001_MIDDLE);
  int right = analogRead(PIN_ITR20001_RIGHT);

  Serial.print("Left: ");
  Serial.println(left);
  Serial.print("Mid: ");
  Serial.println(mid);
  Serial.print("Right: ");
  Serial.println(right);
  //delay(1000);

  float dist = ultraSound->getDistance();
  Serial.print("Ultrasound: ");
  Serial.println(dist);

  if (left > 200 || mid > 200 || right > 200) {
    int base_speed = 80; // 80 100 140 in no close turns
    int error = right-left;
    int correction = KP * error + KD * (error - last_error);

    int left_speed = base_speed - correction;
    int right_speed = base_speed + correction;

    left_speed = constrain(left_speed, 0, 130);
    right_speed = constrain(right_speed, 0, 130);

    Serial.print(left_speed);
    Serial.print("\n");
    Serial.print(right_speed);
    Serial.print("\n");

    analogWrite(PIN_Motor_PWMA, left_speed);
    analogWrite(PIN_Motor_PWMB, right_speed);
  }
}
