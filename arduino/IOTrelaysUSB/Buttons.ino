/* 
 * IOTrelaysUSB - buttons
 * Button Control
 * Hardware: Sparkfun Pro Micro
 * Version 0.80
 * 16-3-2024
 * Copyright: Ivo Helwegen
 */

#include "Buttons.h"

CButtons::CButtons() { // constructor
  pinMode(BUTTON0_PIN, INPUT_PULLUP);
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pinMode(BUTTON3_PIN, INPUT_PULLUP);
}

void CButtons::init() {
  buttonKeys[0] = BUTTON1;
  buttonKeys[1] = BUTTON2;
  buttonKeys[2] = BUTTON3;
  buttonKeys[3] = BUTTON4;
  vars[0].time = timers.millis();
  vars[1].time = timers.millis();
  vars[2].time = timers.millis();
  vars[3].time = timers.millis();
  vars[0].state = (byte)getInput(0);
  vars[1].state = (byte)getInput(1);
  vars[2].state = (byte)getInput(2);
  vars[3].state = (byte)getInput(3);
  vars[0].lastInput = false;
  vars[1].lastInput = false;
  vars[2].lastInput = false;
  vars[3].lastInput = false;
}

void CButtons::handle() {
  for (int i = 0; i < BUTTONS; i++) {
    handleButton(i);
  }
}

void CButtons::status() {
  sub(0);
  sub(1);
  sub(2);
  sub(3);
}

void CButtons::set(byte buttonNum, byte state) {
  if (buttonNum < BUTTONS) {
    if (!isSwitch(buttonNum)) {
      vars[buttonNum].state = state;
      sub(buttonNum);
    }
  }
}

void CButtons::sub(byte buttonNum) {
  control.subscribe(buttonKeys[buttonNum], (vars[buttonNum].state == relays.off) ? false:true);
}

// Privates

void CButtons::handleButton(byte buttonNum) {
  if (updated(buttonNum)) {
    //control.printf("Updated: " + String(buttonNum) + ", " + String(vars[buttonNum].state));
    control.subscribe(buttonKeys[buttonNum], (vars[buttonNum].state == relays.off) ? false:true);
    control.setButtonState(buttonNum, vars[buttonNum].state);
    //control.printf("Subscribed");
  }
}

bool CButtons::isSwitch(byte buttonNum) {
  if (buttonNum == 0) {
    return (bool)settings.getByte(settings.Buttonsw0);
  } else if (buttonNum == 1) {
    return (bool)settings.getByte(settings.Buttonsw1);
  } else if (buttonNum == 2) {
    return (bool)settings.getByte(settings.Buttonsw2);
  } else {
    return (bool)settings.getByte(settings.Buttonsw3);
  }
}

bool CButtons::getInput(byte buttonNum) {
  if (buttonNum == 0) {
    return !(bool)digitalRead(BUTTON0_PIN);
  } else if (buttonNum == 1) {
    return !(bool)digitalRead(BUTTON1_PIN);
  } else if (buttonNum == 2) {
    return !(bool)digitalRead(BUTTON2_PIN);
  } else {
    return !(bool)digitalRead(BUTTON3_PIN);
  }
}

bool CButtons::updated(byte buttonNum) {
  bool updated = false;
  bool input = getInput(buttonNum);
  if (input != vars[buttonNum].lastInput) {
    if (timers.millis() - vars[buttonNum].time > DEBOUNCE_TIME) {
      if (isSwitch(buttonNum)) {
        vars[buttonNum].state = input ? relays.lock:relays.off;
        updated = true;
      } else if (!input) {
        if ((buttonNum == 0) && (timers.millis() - vars[buttonNum].time > RESETSW_TIME)) {
          control.reset();
        } else if (timers.millis() - vars[buttonNum].time > LOCK_TIME) {
          vars[buttonNum].state = relays.lock;
          updated = true;
        } else {
          vars[buttonNum].state = (vars[buttonNum].state == relays.off) ? relays.on:relays.off;
          updated = true;
        }
      }
    }
    vars[buttonNum].time = timers.millis();
    vars[buttonNum].lastInput = input;
  }
  return updated;
}

CButtons buttons;
