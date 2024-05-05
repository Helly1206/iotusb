/* 
 * IOTrelaysUSB - buttons
 * Nutton Control
 * Hardware: Sparkfun Pro Micro
 * Version 0.80
 * 16-3-2024
 * Copyright: Ivo Helwegen
 */

#ifndef Buttons_h
#define Buttons_h

#define DEBOUNCE_TIME    50   /* ms */
#define LOCK_TIME        1000 /* ms */
#define RESETSW_TIME     3000 /* ms, only reset by button0 */
#define BUTTONS          4
#define BUTTON1          F("button1")
#define BUTTON2          F("button2")
#define BUTTON3          F("button3")
#define BUTTON4          F("button4")

typedef struct {
  byte state;
  bool lastInput;
  unsigned long time;
} buttonVars;

class CButtons {
public:
  CButtons(); // constructor
  void init();
  void handle();
  void status();
  void set(byte buttonNum, byte state);
  void sub(byte buttonNum);
private:
  void handleButton(byte buttonNum);
  bool isSwitch(byte buttonNum);
  bool getInput(byte buttonNum);
  bool updated(byte buttonNum);
  String buttonKeys[BUTTONS];
  buttonVars vars[BUTTONS];
};

extern CButtons buttons;

#endif