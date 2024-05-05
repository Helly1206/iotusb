/* 
 * IOTrelaysUSB - relays
 * Relays control
 * Hardware: Sparkfun Pro Micro
 * Version 0.80
 * 16-3-2024
 * Copyright: Ivo Helwegen
 */

 #include "Relays.h"

cRelays::cRelays() { // constructor
  pinMode(SETRELAY0_PIN, OUTPUT);
  pinMode(RESETRELAY0_PIN, OUTPUT);
  pinMode(SETRELAY1_PIN, OUTPUT);
  pinMode(RESETRELAY1_PIN, OUTPUT);
  pinMode(SETRELAY2_PIN, OUTPUT);
  pinMode(RESETRELAY2_PIN, OUTPUT);
  pinMode(SETRELAY3_PIN, OUTPUT);
  pinMode(RESETRELAY3_PIN, OUTPUT);
}

cRelays::~cRelays() { // destructor
}

void cRelays::init() {
  relayKeys[0] = RELAY1;
  relayKeys[1] = RELAY2;
  relayKeys[2] = RELAY3;
  relayKeys[3] = RELAY4;
  relayTimers[0].SetPeriod(LATCH_TIME);
  relayTimers[1].SetPeriod(LATCH_TIME);
  relayTimers[2].SetPeriod(LATCH_TIME);
  relayTimers[3].SetPeriod(LATCH_TIME);
  relayTimers[0].setCallback(Clear0);
  relayTimers[1].setCallback(Clear1);
  relayTimers[2].setCallback(Clear2);
  relayTimers[3].setCallback(Clear3);
  timers.add(relayTimers[0]);
  timers.add(relayTimers[1]);
  timers.add(relayTimers[2]);
  timers.add(relayTimers[3]);
  setPos(relayKeys[0], false);
  setPos(relayKeys[1], false);
  setPos(relayKeys[2], false);
  setPos(relayKeys[3], false);
  Clear0();
  Clear1();
  Clear2();
  Clear3();
}

void cRelays::setPos(String &key, bool value) {
  byte relayNum = findRelay(key);
  if (relayNum < RELAYS) {
    if (!locked[relayNum]) {
      setPos(relayNum, (byte)(value ? on:off), true);
    } else {
      control.buttonState(relayNum);
      control.subscribe(relayKeys[relayNum], pos[relayNum]);
    }
  }
}

void cRelays::setPos(byte relayNum, byte relayState) {
  setPos(relayNum, relayState, false);
}
  
bool cRelays::getPos(byte relayNum) {
  return pos[relayNum];
}

void cRelays::status() {
  control.subscribe(relayKeys[0], pos[0]);
  control.subscribe(relayKeys[1], pos[1]);
  control.subscribe(relayKeys[2], pos[2]);
  control.subscribe(relayKeys[3], pos[3]);
}

// Privates

void cRelays::setPos(byte relayNum, byte relayState, bool update) {
  if (relayNum < RELAYS) {
    locked[relayNum] = (relayState == lock);
    pos[relayNum] = (relayState == off) ? false:true;
#ifdef RELAYSENABLED
    if (isLatch(relayNum)) {
      setPosLatch(relayNum, pos[relayNum]);
    } else {
      setPosCont(relayNum, pos[relayNum]);
    }
#endif
    control.setRelayState(relayNum, relayState, update);
    control.subscribe(relayKeys[relayNum], pos[relayNum]);
  }
}

byte cRelays::findRelay(String &Name) {
  byte relayNum = 255;
  if (Name == relayKeys[0]) {
    relayNum = 0;
  } else if (Name == relayKeys[1]) {
    relayNum = 1;
  } else if (Name == relayKeys[2]) {
    relayNum = 2;
  } else if (Name == relayKeys[3]) {
    relayNum = 3;
  }
  return relayNum;
}

bool cRelays::isLatch(byte relayNum) {
  if (relayNum == 0) {
    return (bool)settings.getByte(settings.Latch0);
  } else if (relayNum == 1) {
    return (bool)settings.getByte(settings.Latch1);
  } else if (relayNum == 2) {
    return (bool)settings.getByte(settings.Latch2);
  } else {
    return (bool)settings.getByte(settings.Latch3);
  }
}

bool cRelays::isInverse(byte relayNum) {
  if (relayNum == 0) {
    return (bool)settings.getByte(settings.Inverse0);
  } else if (relayNum == 1) {
    return (bool)settings.getByte(settings.Inverse1);
  } else if (relayNum == 2) {
    return (bool)settings.getByte(settings.Inverse2);
  } else {
    return (bool)settings.getByte(settings.Inverse3);
  }
}

void cRelays::setPosCont(byte relayNum, bool value) {
  bool setval = value ^ isInverse(relayNum);
  if (relayNum == 0) {
    digitalWrite(SETRELAY0_PIN, setval);
    digitalWrite(RESETRELAY0_PIN, LOW);
  } else if (relayNum == 1) {
    digitalWrite(SETRELAY1_PIN, setval);
    digitalWrite(RESETRELAY1_PIN, LOW);
  } else if (relayNum == 2) {
    digitalWrite(SETRELAY2_PIN, setval);
    digitalWrite(RESETRELAY2_PIN, LOW);
  } else {
    digitalWrite(SETRELAY3_PIN, setval);
    digitalWrite(RESETRELAY3_PIN, LOW);
  }
}

void cRelays::setPosLatch(byte relayNum, bool value) {
  bool setval = value ^ isInverse(relayNum);
  if (relayNum == 0) {
    if (setval) {
      digitalWrite(SETRELAY0_PIN, HIGH);
      digitalWrite(RESETRELAY0_PIN, LOW);
    } else {
      digitalWrite(SETRELAY0_PIN, LOW);
      digitalWrite(RESETRELAY0_PIN, HIGH);
    }
  } else if (relayNum == 1) {
    if (setval) {
      digitalWrite(SETRELAY1_PIN, HIGH);
      digitalWrite(RESETRELAY1_PIN, LOW);
    } else {
      digitalWrite(SETRELAY1_PIN, LOW);
      digitalWrite(RESETRELAY1_PIN, HIGH);
    }
  } else if (relayNum == 2) {
    if (setval) {
      digitalWrite(SETRELAY2_PIN, HIGH);
      digitalWrite(RESETRELAY2_PIN, LOW);
    } else {
      digitalWrite(SETRELAY2_PIN, LOW);
      digitalWrite(RESETRELAY2_PIN, HIGH);
    }
  } else {
    if (setval) {
      digitalWrite(SETRELAY3_PIN, HIGH);
      digitalWrite(RESETRELAY3_PIN, LOW);
    } else {
      digitalWrite(SETRELAY3_PIN, LOW);
      digitalWrite(RESETRELAY3_PIN, HIGH);
    }
  }
  relayTimers[relayNum].start();
}

void cRelays::Clear0() {
  digitalWrite(SETRELAY0_PIN, LOW);
  digitalWrite(RESETRELAY0_PIN, LOW);
}

void cRelays::Clear1() {
  digitalWrite(SETRELAY1_PIN, LOW);
  digitalWrite(RESETRELAY1_PIN, LOW);
}

void cRelays::Clear2() {
  digitalWrite(SETRELAY2_PIN, LOW);
  digitalWrite(RESETRELAY2_PIN, LOW);
}

void cRelays::Clear3() {
  digitalWrite(SETRELAY3_PIN, LOW);
  digitalWrite(RESETRELAY3_PIN, LOW);
}

cRelays relays;