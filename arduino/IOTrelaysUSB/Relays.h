/* 
 * IOTrelaysUSB - relays
 * Relays control
 * Hardware: Sparkfun Pro Micro
 * Version 0.80
 * 16-3-2024
 * Copyright: Ivo Helwegen
 */

#ifndef Relays_h
#define Relays_h

#define RELAYSENABLED
#define LATCH_TIME    1000 /* ms */
#define RELAYS        4
#define RELAY1        F("relay1")
#define RELAY2        F("relay2")
#define RELAY3        F("relay3")
#define RELAY4        F("relay4")

class cRelays {
  public:
    enum state {off = 0, on = 1, lock = 255};
    cRelays(); // constructor
    ~cRelays(); // destructor
    void init();
    void setPos(String &key, bool value);
    void setPos(byte relayNum, byte relayState);
    bool getPos(byte relayNum);
    void status();
  private:
    void setPos(byte relayNum, byte relayState, bool update);
    byte findRelay(String &Name);
    bool isLatch(byte relayNum);
    bool isInverse(byte relayNum);
    void setPosCont(byte relayNum, bool value);
    void setPosLatch(byte relayNum, bool value);
    static void Clear0();
    static void Clear1();
    static void Clear2();
    static void Clear3();
    String relayKeys[RELAYS];
    Timer relayTimers[RELAYS];
    bool pos[RELAYS];
    bool locked[RELAYS];
};

extern cRelays relays;

#endif