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
#define PIN_Motor_PWMA 5 // Right motor

#define PIN_Motor_BIN_1 8
#define PIN_Motor_PWMB 6 // Left motor

#define KP 0.24 // Last 0.12 0.18 -- Perfect: 0.24 in class 0.01 another 0.0045
#define KD 0.012 // Last 0.04 -- Perfect: 0.04 in class 0.08 another 0.053

#define VEL_BASE 60
#define MAX_VEL 60
#define MIN_VEL 30

#define VALUE_LINE 200 // House 200 -- Labs 400

CRGB leds[NUM_LEDS];
ThreadController controller = ThreadController();
UltraSound* ultraSound;
MessageThread* msg_sender;

int ACK_COUNTER = 1;

int left_speed = 0;
int right_speed = 0;
int last_error = 0;

int start_ = 1;
int end_ = 0;
unsigned long start_time_ = 0;

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
  Serial.begin(9600);

  FastLED.addLeds<NEOPIXEL, PIN_RBGLED>(leds, NUM_LEDS);
  FastLED.setBrightness(20);

  FastLED.showColor(Color(255, 0, 0));
  ackWiFi();
  FastLED.showColor(Color(255, 0, 0));
  ackMQTT();
  FastLED.showColor(Color(0, 255, 0));

  ultraSound = new UltraSound(TRIG_PIN, ECHO_PIN);
  ultraSound->setInterval(12);

  msg_sender = new MessageThread();
  msg_sender->setStartTime(millis());
  msg_sender->setInterval(4000);

  pinMode(PIN_Motor_AIN_1, OUTPUT);
  pinMode(PIN_Motor_BIN_1, OUTPUT);

  pinMode(PIN_Motor_PWMA, OUTPUT);
  pinMode(PIN_Motor_PWMB, OUTPUT);

  pinMode(PIN_Motor_STBY, OUTPUT);
  digitalWrite(PIN_Motor_STBY, HIGH);
  digitalWrite(PIN_Motor_AIN_1, HIGH);
  digitalWrite(PIN_Motor_BIN_1, HIGH);

  start_time_ = millis();
  msg_sender->run();
  controller.add(msg_sender);
  controller.add(ultraSound);
}

void loop() {
  controller.run();

  if (start_ == 0) {
    Serial.print("{ 'strl': " + String(ACK_COUNTER) + " }");
    start_time_ = millis();
    start_ = 1;
  }

  int left = analogRead(PIN_ITR20001_LEFT);
  int mid = analogRead(PIN_ITR20001_MIDDLE);
  int right = analogRead(PIN_ITR20001_RIGHT);

  float dist = ultraSound->getDistance();

  if (ultraSound->getDistance() < 12 && ultraSound->getDistance() > 1) {
    analogWrite(PIN_Motor_PWMA, 0);
    analogWrite(PIN_Motor_PWMB, 0);
    if (end_ == 0) {
      Serial.print("{ 'endl': " + String(millis() - start_time_) + " }");
      Serial.print("{ 'obst': " + String(ultraSound->getDistance()) + " }");
      controller.remove(msg_sender);
      end_ = 1;
    }
  } else {
    if (left > VALUE_LINE || mid > VALUE_LINE || right > VALUE_LINE) { // In line movements
      FastLED.showColor(Color(0, 255, 0));
      int base_speed = 250;
      int error = right-left;
      int correction = KP * error + KD * (error - last_error);

      left_speed = base_speed + correction;
      right_speed = base_speed - correction;

      last_error = error;

      left_speed = constrain(left_speed, 0, 170); // 130 perfect 150 -- 170 risky and 180 200 more risky
      right_speed = constrain(right_speed, 0, 170); // 130 150 -- 170 risky and 180 200 more risky

      analogWrite(PIN_Motor_PWMA, right_speed);
      analogWrite(PIN_Motor_PWMB, left_speed);
    } else if (left < VALUE_LINE && mid < VALUE_LINE && right < VALUE_LINE) { // Lost line movements
      FastLED.showColor(Color(255, 0, 0));
      Serial.print("{ 'line': " + String(1) + " }"); // Send to the ESP32 Lost line
      if (last_error < 0) { // turn right
        analogWrite(PIN_Motor_PWMA, 200);
        analogWrite(PIN_Motor_PWMB, 0);
      } else { // turn left
        analogWrite(PIN_Motor_PWMA, 0);
        analogWrite(PIN_Motor_PWMB, 200);
      }
    }
    if (end_ == 1) {
      msg_sender->setStartTime(millis());
      controller.add(msg_sender);
      start_ = 0;
    }
    end_ = 0;
  }
}
