/* 
 * IOTrelaysUSB - LEDs
 * Switching the LED(s)
 * Hardware: Sparkfun Pro Micro
 * Version 0.80
 * 16-3-2024
 * Copyright: Ivo Helwegen
 */

#ifndef IOTLEDs_h
#define IOTLEDs_h

//#define USE_SYSTEM_LED
#define T_TIME1   500  /* ms */
#define T_TIME2   200 /* ms */
#define LEDS      4

typedef struct { 
  bool state;
  bool buttonState;
  byte count;
  Timer timer;
} ledVars;

class Cled {
public:
  Cled();
  void init();
  void on(byte ledNum);
  void off(byte ledNum);
  void lock(byte ledNum);
  void flash3();
  void flash3(byte ledNum);
private:
  static void handle0();
  static void handle1();
  static void handle2();
  static void handle3();
  static void handle(byte ledNum);
  static void setLED(byte ledNum, byte value);
  static ledVars vars[LEDS];
};

extern Cled LED;

#endif