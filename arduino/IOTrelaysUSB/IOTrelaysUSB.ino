/* 
 * IOTrelaysUSB - IOTrelaysUSB
 * Control up to 4 relays over USB
 * Hardware: Sparkfun Pro Micro
 * Version 0.80
 * 10-3-2024
 * Copyright: Ivo Helwegen
 */

#define APPVERSION       "v0.80"
#define TYPE             "4relays"
#define REVISION         1
//#define FORCE_DEFAULTS

#define BUTTON0_PIN      3 // IN INT0 interrupt
#define BUTTON1_PIN      2 // IN INT1 interrupt
#define BUTTON2_PIN      0 // IN INT2 interrupt
#define BUTTON3_PIN      1 // IN INT3 interrupt
#define LED0_PIN         7 // OUT
#define LED1_PIN         6 // OUT
#define LED2_PIN         5 // OUT
#define LED3_PIN         4 // OUT
#define SETRELAY0_PIN   10 // OUT
#define RESETRELAY0_PIN 16 // OUT
#define SETRELAY1_PIN   14 // OUT
#define RESETRELAY1_PIN 15 // OUT
#define SETRELAY2_PIN   18 // OUT
#define RESETRELAY2_PIN 19 // OUT
#define SETRELAY3_PIN   20 // OUT
#define RESETRELAY3_PIN 21 // OUT

#define LOOPTIMEAPP     100

#include "Timers.h"
#include "Settings.h"
#include "Relays.h"
#include "Buttons.h"
#include "LEDs.h"
#include "Control.h"
#include "Power.h"

void setup() {
  timers.init(LOOPTIMEAPP);
  settings.init();
  relays.init();
  LED.init();
  control.init();
  buttons.init();
  power.init();
}

void loop() {
  timers.handle();
  control.handle();
  buttons.handle();
  power.handle();
}