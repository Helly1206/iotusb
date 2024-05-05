/*
 * IOTrelaysUSB - Settings
 * Reads and writes settings from EEPROM
 * Hardware: Lolin S2 Mini
 * Version 0.80
 * 10-3-2024
 * Copyright: Ivo Helwegen
 */

#include "Settings.h"
#include "Defaults.h"

Item::Item(byte _datatype, unsigned short _start) { // constructor
  datatype = _datatype;
  start = _start;
  size = settings.getSize(_datatype);
}

Item::Item(byte _datatype, unsigned short _start, unsigned short _size) { // constructor
  datatype = _datatype;
  start = _start;
  size = _size;
}

cSettings::cSettings() { // constructor
  initParameters();
}

cSettings::~cSettings() { // destructor
  EEPROM.end();
}

void cSettings::init() {
#ifdef FORCE_DEFAULTS
  resetSettings(EEPROM_START, memsize);
#endif
  if (IsEmpty(EEPROM_START, memsize)) {
    //control.printf("Settings: No settings, loading default");
    defaultParameters();
  }
}

void cSettings::get(Item *item, byte &b) {
  if (item->datatype == DT_BYTE) {
    EEPROM.get(item->start, b);
  }
}

void cSettings::get(Item *item, unsigned short &s) {
  if (item->datatype == DT_SHORT) {
    EEPROM.get(item->start, s);
  }
}

void cSettings::get(Item *item, unsigned long &l) {
  if (item->datatype == DT_LONG) {
    EEPROM.get(item->start, l);
  }
}

void cSettings::get(Item *item, float &f) {
  if (item->datatype == DT_FLOAT) {
    EEPROM.get(item->start, f);
  }
}

void cSettings::get(Item *item, String &s) {
  if (item->datatype == DT_STRING) {
    char c = '-';
    s.reserve(item->size + 2);
    s = "";
    for (unsigned short i = 0; ((i < item->size) && (c != '\0')); i++) {
      c = char(EEPROM.read(item->start + i));
      if (c != '\0') {
        s += c;
      }
    }
  }
}

void cSettings::get(Item *item, char *s) {
  if (item->datatype == DT_STRING) {
    char c = '-';
    for (unsigned short i = 0; (i < item->size); i++) {
      s[i] = char(EEPROM.read(item->start + i));
    }
  }
}

byte cSettings::getByte(Item *item) {
  byte b;
  get(item, b);
  return b;
}

unsigned short cSettings::getShort(Item *item) {
  unsigned short s;
  get(item, s);
  return s;
}

unsigned long cSettings::getLong(Item *item) {
  unsigned long l;
  get(item, l);
  return l;
}

float cSettings::getFloat(Item *item) {
  float f;
  get(item, f);
  return f;
}

String cSettings::getString(Item *item) {
  String s;
  get(item, s);
  return s;
}

void cSettings::set(Item *item, byte &b) {
  if (item->datatype == DT_BYTE) {
    EEPROM.put(item->start, b);
  }
}

void cSettings::set(Item *item, unsigned short &s) {
  if (item->datatype == DT_SHORT) {
    EEPROM.put(item->start, s);
  }
}

void cSettings::set(Item *item, unsigned long &l) {
  if (item->datatype == DT_LONG) {
    EEPROM.put(item->start, l);
  }
}

void cSettings::set(Item *item, float &f) {
  if (item->datatype == DT_FLOAT) {
    EEPROM.put(item->start, f);
  }
}

void cSettings::set(Item *item, String &s) {
  if (item->datatype == DT_STRING) {
    if (s.length() > item->size) {
      s = s.substring(0, item->size);
    }
    unsigned short i;
    for (i = 0; i < s.length(); i++) {
      EEPROM.write(item->start + i, s[i]);
    }
    for (i = s.length(); i < item->size; i++) {
      EEPROM.write(item->start + i, 0);
    }
  }
}

void cSettings::set(Item *item, char *s) {
  if (item->datatype == DT_STRING) {
    unsigned short i;
    for (i = 0; i < item->size; i++) {
      EEPROM.write(item->start + i, s[i]);
    }
  }
}

void cSettings::update() {
  //EEPROM.commit();
}

unsigned short cSettings::getSize(byte datatype) {
  unsigned short dsize = 0;
  switch (datatype) {
    case DT_BYTE:
      dsize = sizeof(byte);
      break;
    case DT_SHORT:
      dsize = sizeof(unsigned short);
      break;
    case DT_LONG:
      dsize = sizeof(unsigned long);
      break;
    case DT_FLOAT:
      dsize = sizeof(float);
      break;
    case DT_STRING:
      dsize = 4;
      break;
    default:
      break;
  }
  return dsize;
}

byte cSettings::getDatatype(Item *item) {
  return item->datatype;
}

String cSettings::getECS() {
  String result;
  CRC16 crc(CRC16_CCITT_POLYNOME, CRC16_CCITT_INITIAL, CRC16_CCITT_XOR_OUT, CRC16_CCITT_REV_IN, CRC16_CCITT_REV_OUT);

  for (int i=startSettings; i < memsize; i++) {
    crc.add(EEPROM.read(i));
  }
  result = String(crc.calc(), HEX);
  result.toUpperCase();
  return result;
}

bool cSettings::setHex(String &data) {
  bool result = false;
  int start = 0;
  int length = 0;
  int pos = data.indexOf(':');
  if (pos != -1) {
    start = data.substring(0,pos).toInt();
    data = data.substring(pos+1);
  }
  data.toUpperCase();
  if ((data.length() % 2) == 0) {
    length = data.length()/2;
    if (start + length <= sizeSettings) {
      result = true;
      for (pos = 0; pos < data.length(); pos++) {
        if (!isHexadecimalDigit(data[pos])) {
          result = false;
        }
      }
    }
  }
  if (result) {
    for (pos = 0; pos < length; pos++) {
      byte val = (Hex2Byte(data[2*pos])<<4) + Hex2Byte(data[2*pos+1]);
      //control.printf(String(String(start + pos) + ":" + String(val) + ":" + data[2*pos] + data[2*pos+1]));
      EEPROM.write(startSettings + start + pos, val);
    }
  }
  return result;
}

String cSettings::getHex(String &data) {
  String result;
  int start = 0;
  int length = 0;
  int pos = 0;
  if (data.length() > 0) {
    pos = data.indexOf(':');
    if (pos == -1) {
      start = data.toInt();
      length = sizeSettings - start;
    } else {
      start = data.substring(0,pos).toInt();
      length = data.substring(pos+1).toInt();
    }
  } else {
    length = sizeSettings;
  }
  if (start + length <= sizeSettings) {
    result.reserve(2*length + 2);
    for (pos = 0; pos < length; pos++) {
      byte val = EEPROM.read(startSettings + start + pos);
      //result += ":" + String(startSettings + start + pos) + ":";
      if (val < 16) {
        result += '0';
      }
      result += String(val, HEX);
    }
  }
  result.toUpperCase();

  return result;
}

///////////// PRIVATES ///////////////////////////

void cSettings::initParameters() {
  unsigned short startAddress = EEPROM_START;

  Name = new Item(DT_STRING, startAddress, STANDARD_SIZE); // String 32
  startAddress += STANDARD_SIZE;
  startSettings = startAddress;
  Enable0 = new Item(DT_BYTE, startAddress);               // [bool]
  startAddress += getSize(DT_BYTE);
  Enable1 = new Item(DT_BYTE, startAddress);               // [bool]
  startAddress += getSize(DT_BYTE);
  Enable2 = new Item(DT_BYTE, startAddress);               // [bool]
  startAddress += getSize(DT_BYTE);
  Enable3 = new Item(DT_BYTE, startAddress);               // [bool]
  startAddress += getSize(DT_BYTE);
  Latch0 = new Item(DT_BYTE, startAddress);                // [bool]
  startAddress += getSize(DT_BYTE);
  Latch1 = new Item(DT_BYTE, startAddress);                // [bool]
  startAddress += getSize(DT_BYTE);
  Latch2 = new Item(DT_BYTE, startAddress);                // [bool]
  startAddress += getSize(DT_BYTE);
  Latch3 = new Item(DT_BYTE, startAddress);                // [bool]
  startAddress += getSize(DT_BYTE);
  Inverse0 = new Item(DT_BYTE, startAddress);              // [bool]
  startAddress += getSize(DT_BYTE);
  Inverse1 = new Item(DT_BYTE, startAddress);              // [bool]
  startAddress += getSize(DT_BYTE);
  Inverse2 = new Item(DT_BYTE, startAddress);              // [bool]
  startAddress += getSize(DT_BYTE);
  Inverse3 = new Item(DT_BYTE, startAddress);              // [bool]
  startAddress += getSize(DT_BYTE);
  Buttonsw0 = new Item(DT_BYTE, startAddress);             // [bool]
  startAddress += getSize(DT_BYTE);
  Buttonsw1 = new Item(DT_BYTE, startAddress);             // [bool]
  startAddress += getSize(DT_BYTE);
  Buttonsw2 = new Item(DT_BYTE, startAddress);             // [bool]
  startAddress += getSize(DT_BYTE);
  Buttonsw3 = new Item(DT_BYTE, startAddress);             // [bool]
  startAddress += getSize(DT_BYTE);
  Lowpower = new Item(DT_BYTE, startAddress);              // [bool]
  startAddress += getSize(DT_BYTE);
  LowpowerLEDs = new Item(DT_BYTE, startAddress);          // [bool]
  startAddress += getSize(DT_BYTE);
  memsize = startAddress;
  sizeSettings = memsize - startSettings;
}

void cSettings::resetSettings(unsigned short start, unsigned short size) {
  byte bval = 0xFF;
  for (int i=0; i < size; i++) {
    EEPROM.write(start + i, bval);
  }
  update();
}

bool cSettings::IsEmpty(unsigned short start, unsigned short size) {
  bool empty = true;
  byte rd = 0;

  for (int i=0; ((i < size) && (empty)); i++) {
    rd = EEPROM.read(start + i);
    if ((rd != 0x00) && (rd != 0xFF)) {
      empty = false;
    }  
  }
  return empty;
}

void cSettings::defaultParameters() {
    byte bval = true;
    String sval;

    set(Name, sval = DEF_NAME);
    set(Enable0, bval = DEF_ENABLE0);
    set(Enable1, bval = DEF_ENABLE1);
    set(Enable2, bval = DEF_ENABLE2);
    set(Enable3, bval = DEF_ENABLE3);
    set(Latch0, bval = DEF_LATCH0);
    set(Latch1, bval = DEF_LATCH1);
    set(Latch2, bval = DEF_LATCH2);
    set(Latch3, bval = DEF_LATCH3);
    set(Inverse0, bval = DEF_INVERSE0);
    set(Inverse1, bval = DEF_INVERSE1);
    set(Inverse2, bval = DEF_INVERSE2);
    set(Inverse3, bval = DEF_INVERSE3);
    set(Buttonsw0, bval = DEF_BUTTONSW0);
    set(Buttonsw1, bval = DEF_BUTTONSW1);
    set(Buttonsw2, bval = DEF_BUTTONSW2);
    set(Buttonsw3, bval = DEF_BUTTONSW3);
    set(Lowpower, bval = DEF_LOWPOWER);
    set(LowpowerLEDs, bval = DEF_LOWPOWERLEDS);
    update();
}

byte cSettings::Hex2Byte(char ch) {
  byte val = 0;
  if ((ch >= 'A') && (ch <= 'F')) {
    val = (ch - 'A' + 10);
  } else if ((ch >= '0') && (ch <= '9')) {
    val = (ch - '0');
  } 
  return val;
}

cSettings settings;
