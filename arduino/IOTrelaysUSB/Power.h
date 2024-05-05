/* 
 * IOTrelaysUSB - Power
 * Low Power Handling
 * Hardware: Sparkfun Pro Micro
 * Version 0.80
 * 17-3-2024
 * Copyright: Ivo Helwegen
 */

#ifndef POWER_h
#define POWER_h

#if defined __AVR_ATmega32U4__
	// Timer 4 PRR bit is currently not defined in iom32u4.h
	#ifndef PRTIM4
		#define PRTIM4 4
	#endif

	// Timer 4 power reduction macro is not defined currently in power.h
	#ifndef power_timer4_disable
		#define power_timer4_disable()	(PRR1 |= (uint8_t)(1 << PRTIM4))
	#endif

	#ifndef power_timer4_enable
		#define power_timer4_enable()		(PRR1 &= (uint8_t)~(1 << PRTIM4))
	#endif
#endif

//#define TESTPWRLED

class CPower {
public:
  CPower(); // constructor
  void init();
  void handle();
  void timer(bool enable);
private:
  void disablePeripherals();
  void idle();
};

extern CPower power;

#endif