/* 
 * IOTrelaysUSB - Power
 * Low Power Handling
 * Hardware: Sparkfun Pro Micro
 * Version 0.80
 * 17-3-2024
 * Copyright: Ivo Helwegen
 */

#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include "Power.h"

CPower::CPower() { // constructor
}

void CPower::init() {
  disablePeripherals();
}

void CPower::handle() {
  if (settings.getByte(settings.Lowpower)) {
    idle();
  }
#ifdef TESTPWRLED
  digitalWrite(LED_BUILTIN_RX, !digitalRead(LED_BUILTIN_RX));
#endif
}

void CPower::timer(bool enable) {
  if (enable) {
    power_timer0_enable(); // timer0 is required to handle delays in serial communication
  } else {
    power_timer0_disable();
  }
}

///////////// PRIVATES ///////////////////////////

void CPower::disablePeripherals() {
  ADCSRA &= ~(1 << ADEN);
	power_adc_disable();
  power_timer0_disable();
  power_timer4_disable();
  power_timer3_disable();
  //power_timer1_disable();
  power_spi_disable();
  power_usart1_disable();
  power_twi_disable();
  //power_usb_disable();
  wdt_disable();
  power_timer1_enable();
  power_usb_enable();
}

void CPower::idle() {
  set_sleep_mode(SLEEP_MODE_IDLE);
  cli();
  sleep_enable();
  sei();
  sleep_cpu();
  sleep_disable();
  sei();
}

CPower power;
