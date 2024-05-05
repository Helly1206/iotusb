/* 
 * IOTrelaysUSB - control
 * Serial and process control
 * Hardware: Sparkfun Pro Micro
 * Version 0.80
 * 10-3-2024
 * Copyright: Ivo Helwegen
 */

/*

command:                      response:
“rst”	                        {"rst": true}
“online?”	                    {“online”: true} implement these?
	                            {“online”: true} implement these?
{“pub”: {"relay0": true}}	
	                            {“sub”: {"relay0": true}}
	                            {“sub”: {"switch0": true}}
"sub"                         subscribe to all (current status will be returned)
“?”	                          {“name”: “name”, “type”: “4relays”, “rev”: 1, “ecs”: "ABCD""} (eeprom checksum crc16)
{“set”: “EF23D5E8”}	          {“set”: “ABCD”}	ecs
{“set”: “8:EF23D5E8”}	        {“set”: “ABCD”}	ecs
{“get”: “0:4”}	              {“get”: “E3A2B6C9”}	Start-address, length
{“get”: “4”}	                {“get”: “E3A2B6C9”}	Start-address
“get”	                        {“get”: “E3A2B6C9……”}	Complete	
{“name”: devname}	            {“name”: devname}	
On error	                    {“error”: “{cmd}: error description}	
"dbg"                         "debug eeprom and status printf, no json just to display in terminal"
"prg"                         "Switch low power off for programming, otherwise watchdog won't allow programming"
*/

#include "Control.h"

Ccontrol::Ccontrol() { // constructor
  commandAvailable = false;
  command.reserve(MAXCOMMANDSIZE);
  command = "";
}

void Ccontrol::init() {
  Serial.begin(BITRATE);
  Serial.flush();
  LED.flash3();
}

void Ccontrol::handle() {  
  if (commandAvailable) {
    handleCommand();
    commandAvailable = false;
    command = "";
  } else if (Serial.available()) {
    char c = Serial.read(); 
    if (c > 0) {
      if (c == '\n') {
        commandAvailable = true;
      } else if (c != '\r') {
        command += c;
      }
    }
  }
}

void Ccontrol::subscribe(String &key, bool value) {
  JSON jString(MAXSUBSIZE);
  JSON jsub(MAXSUBCMDSIZE);
  String cmd = F("sub");
  jsub.AddItem(key, value);
  jString.AddItem(cmd, jsub);
  printf(jString.GetJson());
}

void Ccontrol::print(const String &data) {
  printf(data);
}

void Ccontrol::reset() {
  printReset();
  timers.delay(RESETDELAY);
  softwareReset::standard(); //simple();
}

void Ccontrol::setRelayState(byte relayNum, byte state, bool update) {
  if (isEnabled(relayNum)) {
    if (state == relays.lock) {
      LED.lock(relayNum);
    } else if (state == relays.on) {
      LED.on(relayNum);
    } else {
      LED.off(relayNum);
    }
    if (update) {
      buttons.set(relayNum, state);
    }
  }
}

void Ccontrol::buttonState(byte relayNum) {
  if (isEnabled(relayNum)) {
    buttons.sub(relayNum);
  }
}

void Ccontrol::setButtonState(byte buttonNum, byte state) {
  if (isEnabled(buttonNum)) {
    relays.setPos(buttonNum, state);
  } else if (state == relays.off) {
    LED.off(buttonNum);
  } else {
    LED.on(buttonNum);
  }
}

// Privates

bool Ccontrol::isEnabled(byte num) {
if (num == 0) {
    return (bool)settings.getByte(settings.Enable0);
  } else if (num == 1) {
    return (bool)settings.getByte(settings.Enable1);
  } else if (num == 2) {
    return (bool)settings.getByte(settings.Enable2);
  } else {
    return (bool)settings.getByte(settings.Enable3);
  }
}

void Ccontrol::handleCommand() {
  JSON jcmd; // Default size
  String cmd;
  jcmd.LoadJson(command);
  do {
    cmd = jcmd.getKey();
    if (cmd == F("rst")) {
      reset();
    } else if (cmd == F("?")) {
      printWho();
    } else if (cmd == F("set")) {
      String setVal = jcmd.getString();
      if (settings.setHex(setVal)) {
        printSet();
      } else {
        String descr = F("Incorrect syntax");
        printError(cmd, descr);
      }
    } else if (cmd == F("get")) {
      String getVal = jcmd.getString();
      printGet(settings.getHex(getVal));
    } else if (cmd == F("name")) {
      String name = jcmd.getString();
      settings.set(settings.Name, name);
      printName();
    } else if (cmd == F("pub")) {
      publish(jcmd);
    } else if (cmd == F("sub")) {
      relays.status();
      buttons.status();
    } else if (cmd == F("dbg")) {
      LED.flash3();
    } else {
      String descr = F("Invalid command");
      printError(cmd, descr);
    }
  } while (jcmd.next());
}

void Ccontrol::printError(String &cmd, String &Description) {
  JSON error(MAXERRORSIZE);
  String errCmd = F("ERROR");
  String errStr;
  errStr.reserve(32);
  errStr = cmd;
  errStr += F(": ");
  errStr += Description;
  error.AddItem(errCmd, errStr);
  printf(error.GetJson());
}

void Ccontrol::printReset() {
  JSON jString(MAXRESETSIZE);
  String cmd = F("rst");
  jString.AddItem(cmd, true);
  printf(jString.GetJson());
}

void Ccontrol::printWho() {
  JSON jString(MAXWHOSIZE);
  String cmd;
  String value;
  cmd = F("name");
  value = getName();
  jString.AddItem(cmd, value);
  cmd = F("type");
  value = F(TYPE);
  jString.AddItem(cmd, value);
  cmd = F("rev");
  jString.AddItem(cmd, REVISION);
  cmd = F("ecs");
  value = settings.getECS();
  jString.AddItem(cmd, value);
  printf(jString.GetJson());
}

void Ccontrol::printSet() {
  JSON jString;
  String cmd = F("set");
  jString.AddItem(cmd, settings.getECS());
  printf(jString.GetJson());
}

void Ccontrol::printGet(String data) {
  String cmd = F("get");
  if (data.length() > 0) {
    JSON jString(MAXSETSIZE);
    jString.AddItem(cmd, data);
    printf(jString.GetJson());
  } else {
    String descr = F("Incorrect syntax");
    printError(cmd, descr);
  }
}

void Ccontrol::printName() {
  JSON jString(MAXNAMESIZE);
  String cmd = F("name");
  jString.AddItem(cmd, getName());
  printf(jString.GetJson());
}

void Ccontrol::publish(JSON &jcmd) {
  if (jcmd.getType() == JT_JSON) {
    JSON jdata = jcmd.getJsonItem(); // keep max size
    do {
      String key = jdata.getKey();
      relays.setPos(key, jdata.getBoolean());
    } while (jdata.next());
  }
}

String Ccontrol::getName() {
  String name;
  settings.get(settings.Name, name);
  if (name.length() == 0) {
    name = EMPTY_NAME;
  }
  return name;
}

void Ccontrol::printDebug() {
  printf("NA");
}

void Ccontrol::printf(const String &data) {
  unsigned long start = timers.millis();
  /* This strange code is required to prevent losing data 'sometimes' during printing
   * I cannot solve the initial issue, but it seems that the serial port doesn't accept
   * more than one call at the time when idle or sleeping or something
   * So, always take care that the buffer is empty, then send '\r' to wake up and afterwards
   * you are free to send your data
   * timeout is to not block when this fails. Even on timeout the system tries writing
   * it may help.
   */
  power.timer(true);
  while ((Serial.availableForWrite() < SERIAL_BUFFER_SIZE) && (timers.millis() - start < TIMEOUTWRITE)) {  
  }
  Serial.print('\r');
  while ((Serial.availableForWrite() < SERIAL_BUFFER_SIZE) && (timers.millis() - start < TIMEOUTWRITE)) {  
  }
  Serial.println(data);
  power.timer(false);
}

Ccontrol control;
