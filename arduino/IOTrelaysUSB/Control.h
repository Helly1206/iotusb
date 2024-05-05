/* 
 * IOTrelaysUSB - control
 * Serial and process control
 * Hardware: Sparkfun Pro Micro
 * Version 0.80
 * 10-3-2024
 * Copyright: Ivo Helwegen
 */

#ifndef control_h
#define control_h

#include <SoftwareReset.hpp>
#include "Json.h"

#define DEBUG_CONTROL

#define EMPTY_NAME      F("<empty>")
#define BITRATE         115200
#define LINEDELAY       100 /* ms, 8 BITS * 128 CHARACTERS / 115200 BPS  = 8.9 ms to prevent buffer overflow */ 
#define RESETDELAY      1000 /* ms */
#define TIMEOUTWRITE    1000 /* ms */
#define MAXCOMMANDSIZE  128
#define MAXSUBSIZE       30 /* {"sub":20} = 28 -> 30 */
#define MAXSUBCMDSIZE    20 /* {{"6":5}} = 18 -> 20 */
#define MAXERRORSIZE     46 /* {"ERROR":"32"} = 44 -> 46 */
#define MAXRESETSIZE     14 /* {"rst":true} = 12 -> 14 */
#define MAXWHOSIZE       86 /* name = 32, rest about 50 */
#define MAXSETSIZE       16 /* {"set":"1234"} = 14 -> 16 */
#define MAXNAMESIZE      45 /* {"name":"32"} = 11 + 32 = 43 -> 45 */ 

class Ccontrol {
public:
  Ccontrol(); // constructor
  void init();
  void handle(); 
  void subscribe(String &key, bool value);
  void print(const String &data);
  void reset();
  void setRelayState(byte relayNum, byte state, bool update);
  void buttonState(byte relayNum);
  void setButtonState(byte buttonNum, byte state);
private:
  String command;
  bool commandAvailable;
  bool isEnabled(byte num);
  void handleCommand();
  void printError(String &cmd, String &Description);
  void printReset();
  void printWho();
  void printSet();
  void printGet(String data);
  void printName();
  void publish(JSON &jcmd);
  String getName();
  void printDebug();
  void printf(const String &data);
};

extern Ccontrol control;

#endif
