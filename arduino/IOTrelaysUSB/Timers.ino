/* 
 * IOTrelaysUSB - timers
 * Timers handling
 * Hardware: Sparkfun Pro Micro
 * Version 0.80
 * 16-3-2024
 * Copyright: Ivo Helwegen
 */

Timer::Timer() { // constructor
  period = 0;
  repeat = false;
  callback = NULL;
  running = false;
  start_time = 0;
} 

Timer::Timer(unsigned long _period, bool _repeat, void (*_callback)()) { // constructor
  period = _period;
  repeat = _repeat;
  callback = _callback;
  running = false;
  start_time = 0;
} 

void Timer::SetPeriod(unsigned long _period) {
  period = _period;
}

void Timer::SetRepeat(bool _repeat) {
  repeat = _repeat;
}

void Timer::setCallback(void (*_callback)()) {
  callback = _callback;
}

void Timer::start() {
  running = true;
  start_time = timers.millis();
}

void Timer::cancel() {
  running = false;
  start_time = 0;
}

bool Timer::isRunning() {
  return running;
}

void Timer::handle() {
  if (running) {
    if (timers.millis() - start_time >= period) {
      if (repeat) {
        start_time += period;
      } else {
        start_time = 0;
        running = false;
      }
      if (callback) {
        callback();
      }
    }
  }
}

static volatile unsigned short looptime = 0;
static volatile unsigned short timercompreg = 0;
static volatile unsigned short timercomp = 0;
static volatile unsigned long milliscomp = 0;

cTimers::cTimers() { // constructor
  ntimers = 0;
}
cTimers::~cTimers() { // destructor
}

void cTimers::init(unsigned short looptimems) {
  setLoopTime(looptimems);
  TCCR1A = 0;            // Init Timer1A
  TCCR1B = 0;            // Init Timer1B
  TCCR1B |= setPrescaler();  // Prescaler
  OCR1A = setTimerCompReg(); // Timer Compare1A Register
  TCNT1 = 0;
  TIMSK1 = 0;
  TIMSK1 |= _BV(OCIE1A); // Enable Timer COMPA Interrupt
}

void cTimers::add(Timer &_timer) {
  if (ntimers < MAX_TIMERS) {
    myTimers[ntimers] = &_timer;
    ntimers++;
  }
}

void cTimers::handle() {
  for (int i = 0; i < ntimers; i++) {
    myTimers[i]->handle();
  }
}

unsigned long cTimers::millis() {
  unsigned long m;
  unsigned short t;
	uint8_t oldSREG = SREG;
  // disable interrupts while we read timer1_millis or we might get an
  // inconsistent value (e.g. in the middle of a write to timer1_millis)
  cli();
  t = TCNT1-timercomp;
  m = milliscomp + (((unsigned long)t)*prescaler)/(F_CPU/1000);
  SREG = oldSREG;
  return m;
}

void cTimers::delay(unsigned long ms) {
	uint32_t start = millis();

	while (millis() - start < ms) {
		yield();
	}
}

// Privates

void cTimers::setLoopTime(unsigned short looptimems) {
  if (looptimems > 4000) {
    looptime = 4000;
  } else {
    looptime = looptimems;
  }
}

byte cTimers::setPrescaler() {
  byte prescalerbits = TMR_PRESC_OFF;
  prescaler = 1;
  if (((F_CPU/1000)*looptime) < TIMER1_RESOLUTION) {
    prescalerbits = TMR_PRESC_DIV1;
  } else if ((((F_CPU/1000)*looptime)/8) < TIMER1_RESOLUTION) {
    prescalerbits = TMR_PRESC_DIV8;
    prescaler = 8;
  } else if ((((F_CPU/1000)*looptime)/64) < TIMER1_RESOLUTION) {
    prescalerbits = TMR_PRESC_DIV64;
    prescaler = 64;
  } else if ((((F_CPU/1000)*looptime)/256) < TIMER1_RESOLUTION) {
    prescalerbits = TMR_PRESC_DIV256;
    prescaler = 256;
  } else if ((((F_CPU/1000)*looptime)/1024) < TIMER1_RESOLUTION) {
    prescalerbits = TMR_PRESC_DIV1024;
    prescaler = 1024;
  }
  return prescalerbits;
}

byte cTimers::setTimerCompReg() {
  timercompreg = (unsigned short)(((F_CPU/1000)*looptime)/(unsigned long)prescaler);
  return timercompreg;
}

ISR(TIMER1_COMPA_vect)
{
  timercomp = OCR1A;
  OCR1A += timercompreg; // Advance The COMPA Register
  milliscomp += looptime;
}

cTimers timers;