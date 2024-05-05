/* 
 * IOTrelaysUSB - LEDs
 * Switching the LED(s)
 * Hardware: Sparkfun Pro Micro
 * Version 0.80
 * 16-3-2024
 * Copyright: Ivo Helwegen
 */

#include "LEDs.h"


/*

start: 3x flash 500 ms all LEDs, DBG: same
no low power LEDs: on = on, off = off
low power LEDs: on = on for 1s
                off = on for 500ms
switch off Rx and Tx LEDs (use RX as system LED for testing)
*/

static ledVars Cled::vars[LEDS];

Cled::Cled() {
#ifdef USE_SYSTEM_LED
  pinMode(LED_BUILTIN_RX, OUTPUT);
  pinMode(LED_BUILTIN_TX, OUTPUT);
  digitalWrite(LED_BUILTIN_RX, HIGH);
  digitalWrite(LED_BUILTIN_TX, HIGH);
#else
  pinMode(LED_BUILTIN_RX, INPUT);
  pinMode(LED_BUILTIN_TX, INPUT);
#endif
  pinMode(LED0_PIN, OUTPUT);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
}

void Cled::init() {
  vars[0].timer.SetPeriod(T_TIME1);
  vars[1].timer.SetPeriod(T_TIME1);
  vars[2].timer.SetPeriod(T_TIME1);
  vars[3].timer.SetPeriod(T_TIME1);
  vars[0].timer.setCallback(handle0);
  vars[1].timer.setCallback(handle1);
  vars[2].timer.setCallback(handle2);
  vars[3].timer.setCallback(handle3);
  timers.add(vars[0].timer);
  timers.add(vars[1].timer);
  timers.add(vars[2].timer);
  timers.add(vars[3].timer);
}

void Cled::on(byte ledNum) {
  vars[ledNum].timer.cancel();
  if (settings.getByte(settings.LowpowerLEDs)) {
    vars[ledNum].buttonState = false;
    vars[ledNum].count = 15;
    setLED(ledNum, HIGH);
    vars[ledNum].timer.SetPeriod(T_TIME2);
    vars[ledNum].timer.start();
  } else {
    vars[ledNum].buttonState = true;
    setLED(ledNum, HIGH);
  }
}

void Cled::off(byte ledNum) {
  vars[ledNum].timer.cancel();
  vars[ledNum].buttonState = false;
  if (settings.getByte(settings.LowpowerLEDs)) {
    vars[ledNum].count = 3;
    setLED(ledNum, HIGH);
    vars[ledNum].timer.SetPeriod(T_TIME2);
    vars[ledNum].timer.start();
  } else {
    setLED(ledNum, LOW);
  }  
}

void Cled::lock(byte ledNum) {
  vars[ledNum].timer.cancel();
  vars[ledNum].buttonState = false;
  vars[ledNum].count = 255;
  setLED(ledNum, HIGH);
  vars[ledNum].timer.SetPeriod(T_TIME2);
  vars[ledNum].timer.start();
}

void Cled::flash3() {
  flash3(0);
  flash3(1);
  flash3(2);
  flash3(3);
}

void Cled::flash3(byte ledNum) {
  vars[ledNum].timer.cancel();
  vars[ledNum].count = 5;
  setLED(ledNum, HIGH);
  vars[ledNum].timer.SetPeriod(T_TIME1);
  vars[ledNum].timer.start();
}

///////////// PRIVATES ///////////////////////////

void Cled::handle0() {
  handle(0);
}

void Cled::handle1() {
  handle(1);
}

void Cled::handle2() {
  handle(2);
}

void Cled::handle3() {
  handle(3);
}

void Cled::handle(byte ledNum) {
  if (vars[ledNum].count > 0) {
    if (vars[ledNum].count < 255) {
      vars[ledNum].count--;
    }
    setLED(ledNum, !vars[ledNum].state);
    vars[ledNum].timer.start();
  } else if (vars[ledNum].buttonState) {
    setLED(ledNum, HIGH);
  } else {
    setLED(ledNum, LOW);
  }
}

void Cled::setLED(byte ledNum, byte value) {
  if (ledNum == 0) {
    digitalWrite(LED0_PIN, value);
#ifdef USE_SYSTEM_LED
    digitalWrite(LED_BUILTIN_RX, !value);
#endif
  } else if (ledNum == 1) {
    digitalWrite(LED1_PIN, value);
#ifdef USE_SYSTEM_LED
    digitalWrite(LED_BUILTIN_TX, !value);
#endif
  } else if (ledNum == 2) {
    digitalWrite(LED2_PIN, value);
  } else {
    digitalWrite(LED3_PIN, value);
  }
  vars[ledNum].state = (bool)value;
}

Cled LED;