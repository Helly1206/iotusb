/* 
 * IOTrelaysUSB - JSON
 * Simple JSON encoder/ decoder
 * Hardware: Sparkfun Pro Micro
 * Version 0.80
 * 10-3-2024
 * Copyright: Ivo Helwegen
 */

#ifndef JSON_h
#define JSON_h

#define JT_UNKNOWN    -1
#define JT_JSON        0
#define JT_STRING      1
#define JT_ARRAY       2
#define JT_INT         3
#define JT_FLOAT       4
#define JT_BOOLEAN     5
#define JT_NULL        6

#define DEFAULTLENGTH 128

//#define NUMITEMS(arg) ((unsigned int) (sizeof (arg) / sizeof (arg [0])))

class JSON {
public:
  JSON();
  JSON(int length);
  String GetJson();
  void AddItem(String &key, JSON &item);
  void AddItem(String &key, String &item);
  void AddItem(String &key, char* item);
  void AddItem(String &key, int item);
  void AddItem(String &key, float item);
  void AddItem(String &key, bool item);
  void AddItem(String &key); //null
  void AddArray(String &key, String item[], int n);
  void Clear();
  void LoadJson(String &jString);
  void LoadJson(char* jString);
  int getType();
  int getBaseType();
  bool next();
  String getKey();
  String getString();
  int getInt();
  float getFloat();
  bool getBoolean();
  JSON getJsonItem();
  int getReserved();
private:
  void AddToJsonString(String &result);
  int getJsonBaseType();
  int getItemType(String &item);
  String getItem();
  int findStartPos();
  int findEndPos(int start);
  String JsonString;
  int baseType;
  bool isParsed;
  int reserved;
};

#endif
