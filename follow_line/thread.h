#ifndef THREAD_H
#define THREAD_H

#include <Thread.h>
#include <DHT.h>
#include <avr/wdt.h>
#include <ThreadController.h>
#include <StaticThreadController.h>

#define DHTTYPE DHT11

class MessageThread : public Thread {
public:
  String message;

  MessageThread() : Thread() {
  }

  bool shouldRun(unsigned long time) {
    return Thread::shouldRun(time);
  }

  void run() {
    Thread::run();
    Serial.print("{ 'ping': " + String(millis()) + " }");
  }

  void setMessage(String _message) {
    message = _message;
  }
};


class LedThread: public Thread {

public:
  int pin, counter;
  bool state;

  LedThread(int _pin): Thread() {
    pin = _pin;
    state = true;
    counter = 0;
    pinMode(pin, OUTPUT);
  }

  bool shouldRun(unsigned long time){
    return Thread::shouldRun(time);
  }

  void run(){
    Thread::run();
    digitalWrite(pin, state ? HIGH : LOW);
    counter = (state == LOW) ? counter + 1 : counter;
    state = !state;
  }

  int getCounter() {
    return counter;
  }

  void resetCounter() {
    counter = 0;
  }
};

class IncrementLedThread : public Thread {

public:
  int pin;
  int brightness;
  unsigned long time_;
  
  IncrementLedThread(int _pin) : Thread() {
    pin = _pin;
    brightness = 0;
    pinMode(pin, OUTPUT);
  }

  bool shouldRun(unsigned long time) {
    return Thread::shouldRun(time);
  }

  void run() {
    Thread::run();
    brightness = min(brightness + 5, 255);
    analogWrite(pin, brightness);
  }

  void resetBrightness() {
    brightness = 0;
    analogWrite(pin, brightness);
  }
};

class UltraSound: public Thread {

public:
  int trigger, echo;
  unsigned long distance;

  UltraSound(int tr_pin, int ec_pin): Thread() {
    trigger = tr_pin;
    echo = ec_pin;
    distance = 0;

    pinMode(trigger, OUTPUT);
    pinMode(echo, INPUT);
    digitalWrite(trigger, LOW);
  }

  bool shouldRun(unsigned long time){
    return Thread::shouldRun(time);
  }

  void run(){
    Thread::run();
    distance = (getPulseTime() * 0.034) / 2;
  }

  long getPulseTime() {
    digitalWrite(trigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger, LOW);

    long time = pulseIn(echo, HIGH);
    return time;
  }

  long getDistance() {
    return distance;
  }

  void resetDistance() {
    distance = 0;
  }
};

class Dht11: public Thread {

public:
  float humdity, temp;
  DHT *dht;

  Dht11(int dht_pin): Thread() {
    dht = new DHT(dht_pin, DHTTYPE);
    dht->begin();
  }

  bool shouldRun(unsigned long time){
    return Thread::shouldRun(time);
  }

  void run(){
    Thread::run();
    humdity = dht->readHumidity();
    temp = dht->readTemperature();
  }

  float getHumidity() {
    return humdity;
  }

  float getTemperature() {
    return temp;
  }
};

#endif