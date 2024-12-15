#ifndef THREAD_H
#define THREAD_H

#include <Thread.h>
#include <ThreadController.h>
#include <StaticThreadController.h>

class MessageThread : public Thread {
public:
  String message;
  unsigned long startTime;

  MessageThread() : Thread() {
  }

  bool shouldRun(unsigned long time) {
    return Thread::shouldRun(time);
  }

  void run() {
    Thread::run();
    Serial.print("{ 'ping': " + String(millis() - startTime) + " }");
  }

  void setStartTime(unsigned long time) {
    startTime = time;
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

#endif