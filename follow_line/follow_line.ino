#include "thread.h"
#include "FastLED.h"
#define PIN_RBGLED 4
#define NUM_LEDS 1
CRGB leds[NUM_LEDS];

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

ThreadController controller = ThreadController();

UltraSound* ultraSound;

uint32_t Color(uint8_t r, uint8_t g, uint8_t b)
{
  return (((uint32_t)r << 16) | ((uint32_t)g << 8) | b);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  FastLED.addLeds<NEOPIXEL, PIN_RBGLED>(leds, NUM_LEDS);
  FastLED.setBrightness(20);
  int r=255,g=255,b=255;
  FastLED.showColor(Color(r, g, b));

  ultraSound = new UltraSound(TRIG_PIN, ECHO_PIN);
  ultraSound->setInterval(100);

  controller.add(ultraSound);

  pinMode(PIN_Motor_AIN_1, OUTPUT);
  pinMode(PIN_Motor_BIN_1, OUTPUT);

  pinMode(PIN_Motor_PWMA, OUTPUT);
  pinMode(PIN_Motor_PWMB, OUTPUT);

  pinMode(PIN_Motor_STBY, OUTPUT);
  digitalWrite(PIN_Motor_STBY, HIGH);
  analogWrite(PIN_Motor_PWMB, 0);
  analogWrite(PIN_Motor_PWMA, 0);
}

void loop() {
  controller.run();

  digitalWrite(PIN_Motor_AIN_1, HIGH);
  digitalWrite(PIN_Motor_BIN_1, LOW);
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
  delay(1000);

  float dist = ultraSound->getDistance();
  Serial.print("Ultrasound: ");
  Serial.println(dist);

  int r=255,g=0,b=0;
  FastLED.showColor(Color(r, g, b));
}
