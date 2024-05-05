/* 
 * IOTrelaysUSB - Settings
 * Reads and writes settings from EEPROM
 * Hardware: Lolin S2 Mini
 * Version 0.80
 * 10-3-2024
 * Copyright: Ivo Helwegen
 */


 /*
 Address:	EEPROM options:	
32	enable0	enable relay 0 button 0 connection
33	enable1	enable relay 1 button 1 connection
34	enable2	enable relay 2 button 2 connection
35	enable3	enable relay 3 button 3 connection
36	latch0	latching relay 0
37	latch1	latching relay 1
38	latch2	latching relay 2
39	latch3	latching relay 3
40	inverse0	inverse (NC) relay 0
41	inverse1	inverse (NC) relay 1
42	inverse2	inverse (NC) relay 2
43	inverse3	inverse (NC) relay 3
44	buttonsw0	button 0 as switch
45	buttonsw1	button 1 as switch
46	buttonsw2	button 2 as switch
47	buttonsw3	button 3 as switch
48	lowpower	use low power mode
49	lowpowerLEDs	use low power mode on LEDs
0	name	Name, 32 chars max

  */

#ifndef IOTSettings_h
#define IOTSettings_h

#include <EEPROM.h>
#include <CRC16.h>

#define EEPROM_SIZE   512
#define EEPROM_START  0
#define STANDARD_SIZE 32

#define DT_BYTE   0
#define DT_SHORT  1
#define DT_LONG   2
#define DT_FLOAT  3
#define DT_STRING 4

class Item {
  public:
    Item(byte _datatype, unsigned short _start); // constructor
    Item(byte _datatype, unsigned short _start, unsigned short _size); // constructor
    byte datatype;
    unsigned short start;
    unsigned short size;
};

class cSettings {
  public:
    cSettings(); // constructor
    ~cSettings(); // destructor
    void init();
    void get(Item *item, byte &b);
    void get(Item *item, unsigned short &s);
    void get(Item *item, unsigned long &l);
    void get(Item *item, float &f);
    void get(Item *item, String &s);
    void get(Item *item, char *s);
    byte getByte(Item *item);
    unsigned short getShort(Item *item);
    unsigned long getLong(Item *item);
    float getFloat(Item *item);
    String getString(Item *item);

    void set(Item *item, byte &b);
    void set(Item *item, unsigned short &s);
    void set(Item *item, unsigned long &l);
    void set(Item *item, float &f);
    void set(Item *item, String &s);
    void set(Item *item, char *s);

    void update();
    unsigned short getSize(byte datatype);
    byte getDatatype(Item *item);
    String getECS();
    bool setHex(String &data); 
    String getHex(String &data);

    // Parameters
    Item *Name;         // String 32
    Item *Enable0;      // [bool]
    Item *Enable1;      // [bool]
    Item *Enable2;      // [bool]
    Item *Enable3;      // [bool]
    Item *Latch0;       // [bool]
    Item *Latch1;       // [bool]
    Item *Latch2;       // [bool]
    Item *Latch3;       // [bool]
    Item *Inverse0;     // [bool]
    Item *Inverse1;     // [bool]
    Item *Inverse2;     // [bool]
    Item *Inverse3;     // [bool]
    Item *Buttonsw0;    // [bool]
    Item *Buttonsw1;    // [bool]
    Item *Buttonsw2;    // [bool]
    Item *Buttonsw3;    // [bool]
    Item *Lowpower;     // [bool]
    Item *LowpowerLEDs; // [bool]

    unsigned short startSettings;
    unsigned short sizeSettings;
    unsigned short memsize;
  private:
    void initParameters();
    void resetSettings(unsigned short start, unsigned short size);
    bool IsEmpty(unsigned short start, unsigned short size);
    void defaultParameters();
    byte Hex2Byte(char ch);
};

extern cSettings settings;

#endif
