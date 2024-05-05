/* 
 * IOTrelaysUSB - timers
 * Timers handling
 * Hardware: Sparkfun Pro Micro
 * Version 0.80
 * 16-3-2024
 * Copyright: Ivo Helwegen
 */

#ifndef Timers_h
#define Timers_h

#define MAX_TIMERS 16
#define TIMER1_RESOLUTION 65536UL  // Timer1 is 16 bit

#define TMR_PRESC_OFF     0
#define TMR_PRESC_DIV1    1
#define TMR_PRESC_DIV8    2
#define TMR_PRESC_DIV64   3
#define TMR_PRESC_DIV256  4
#define TMR_PRESC_DIV1024 5
#define TMR_PRESC_T0F     6
#define TMR_PRESC_T0R     7

class Timer {
  public:
    Timer(); // constructor
    Timer(unsigned long _period, bool _repeat, void (*_callback)()); // constructor
    void SetPeriod(unsigned long _period);
    void SetRepeat(bool _repeat);
    void setCallback(void (*_callback)());
    void start();
    void cancel();
    bool isRunning();
    void handle();
  private:
    unsigned long period;
    bool repeat;
    void (*callback)();
    bool running;
    unsigned long start_time;
};

class cTimers {
  public:
    cTimers(); // constructor
    ~cTimers(); // destructor
    void init(unsigned short looptimems);
    void add(Timer &_timer);
    void handle();
    unsigned long millis();
    void delay(unsigned long ms);
  private:
    void setLoopTime(unsigned short looptimems);
    byte setPrescaler();
    byte setTimerCompReg();
    Timer* myTimers[MAX_TIMERS];
    byte ntimers;
    unsigned short prescaler;
};

extern cTimers timers;

#endif