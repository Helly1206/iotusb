/* 
 * IOTrelaysUSB - JSON
 * Simple JSON encoder/ decoder
 * Hardware: Sparkfun Pro Micro
 * Version 0.80
 * 10-3-2024
 * Copyright: Ivo Helwegen
 */

#include "Json.h"

JSON::JSON() {
  JSON(DEFAULTLENGTH);
}

JSON::JSON(int length) {
  reserved = length;
  JsonString.reserve(reserved);
  Clear();
}

String JSON::GetJson() {
  if ((isParsed) || (baseType != JT_JSON)) {
    return JsonString;
  } else {
    String result;
    result.reserve(JsonString.length()+4);
    result = '{';
    result += JsonString;
    result += '}';
    return result;
  }
}

void JSON::AddItem(String &key, JSON &item) {
  if ((key.length() == 0) || (baseType != JT_JSON) || (isParsed)) {
    return;
  }
  String result;
  result.reserve(key.length() + item.getReserved() + 5);
  result = '"';
  result += key;
  result += "\":";
  result += item.GetJson();
  AddToJsonString(result);
}

void JSON::AddItem(String &key, String &item) {
  String result;
  if ((key.length() == 0) && (JsonString.length() == 0)) {
    result.reserve(item.length() + 4);
    baseType = JT_STRING;
    result = '"';
    result += item;
    result += '"';
  } else if ((key.length() == 0) || (baseType != JT_JSON) || (isParsed)) {
    return;
  } else {
    result.reserve(key.length() + item.length() + 7);
    result = '"';
    result += key;
    result += "\":\"";
    result += item;
    result += '"';
  }
  AddToJsonString(result);
}

void JSON::AddItem(String &key, char* item) {
  String result;
  if ((key.length() == 0) && (JsonString.length() == 0)) {
    result.reserve(strlen(item) + 4);
    baseType = JT_STRING;
    result = '"';
    result += String(item);
    result += '"';
  } else if ((key.length() == 0) || (baseType != JT_JSON) || (isParsed)) {
    return;
  } else { 
    result.reserve(key.length() + strlen(item) + 7);
    result = '"';
    result += key;
    result += "\":\"";
    result += String(item);
    result += '"';
  }
  AddToJsonString(result);
}

void JSON::AddItem(String &key, int item) {
  if ((key.length() == 0) || (baseType != JT_JSON) || (isParsed)) {
    return;
  }
  String result;
  String sItem = String(item);
  result.reserve(key.length() + sItem.length() + 5);
  result = '"';
  result += key;
  result += "\":";
  result += sItem;
  AddToJsonString(result);   
}

void JSON::AddItem(String &key, float item) {
  if ((key.length() == 0) || (baseType != JT_JSON) || (isParsed)) {
    return;
  }
  String result;
  String sItem = String(item);
  result.reserve(key.length() + sItem.length() + 5);
  result = '"';
  result += key;
  result += "\":";
  result += sItem;
  AddToJsonString(result);
}

void JSON::AddItem(String &key, bool item) { 
  if ((key.length() == 0) || (baseType != JT_JSON) || (isParsed)) {
    return;
  }
  String result;
  String sItem = "false";
  if (item) {
    sItem = "true";
  }
  result.reserve(key.length() + sItem.length() + 5);
  result = '"';
  result += key;
  result += "\":";
  result += sItem;
  AddToJsonString(result);
}

void JSON::AddItem(String &key) {
  if ((key.length() == 0) || (baseType != JT_JSON) || (isParsed)) {
    return;
  }
  String result;
  String sItem = "null";
  result.reserve(key.length() + sItem.length() + 5);
  result = '"';
  result += key;
  result += "\":";
  result += sItem;
  AddToJsonString(result);
}

void JSON::AddArray(String &key, String item[], int n) {
  String result;
  result.reserve(getReserved());
  if ((key.length() == 0) && (JsonString.length() == 0)) {
    baseType = JT_ARRAY;
  } else if ((baseType != JT_JSON) || (key.length() == 0) || (isParsed)) {
    return;
  } else {
    result = '"';
    result += key;
    result += "\":";
  }
  result += '[';
  for (int i = 0; i < n; i++) {
    if (i > 0) {
      result += ",";
    }
    if ((item[i].charAt(0) == '{') || (item[i].charAt(0) == '[')) {
      result += item[i];
    } else {
      result += '"';
      result += item[i];
      result += '"';
    }
  }
  result += "]";
  AddToJsonString(result);
}

void JSON::Clear() {
  JsonString = "";
  baseType = JT_JSON;
  isParsed = false;
}

void JSON::LoadJson(String &jString) {
  Clear();
  JsonString = jString;
  JsonString.trim();
  getJsonBaseType();
  isParsed = true;
}

void JSON::LoadJson(char* jString) {
  Clear();
  JsonString = String(jString);
  JsonString.trim();
  getJsonBaseType();
  isParsed = true;
}

int JSON::getType() {
  int type = JT_UNKNOWN;
  if (isParsed) {
    String subStr = getItem();
    type = getItemType(subStr);
  }
  return type;
}

int JSON::getBaseType() {
  return baseType;
}

bool JSON::next() {
  bool retval = false;
  String item = "";
  int start = -1;
  int end = -1;
  start = findStartPos();
  if (start != -1) {
    end = findEndPos(start);
  }
  if ((start != -1) && (end != -1)) {
    if (baseType == JT_JSON) {
      if (JsonString[end] != '}') {
        String subStr = JsonString.substring(end + 1);
        JsonString = '{';
        JsonString += subStr;
        JsonString.trim();
        retval = true;
      }
    } else if (baseType == JT_ARRAY) {
      if (JsonString[end] != ']') {
        String subStr = JsonString.substring(end + 1);
        JsonString = '[';
        JsonString += subStr;
        JsonString.trim();
        retval = true;
      }
    }
  }
  return retval;
}

String JSON::getKey() {
  String key = "";
  int start = -1;
  int len = -1;
  if (((baseType == JT_JSON) || (baseType == JT_STRING)) && (isParsed)) {
    start = JsonString.indexOf('"') + 1;
    if (start > 0) {
      len = JsonString.substring(start).indexOf('"');
    }
  }
  if ((start > 0) && (len != -1)) {
    key = JsonString.substring(start, start + len);
  }
  return key;
}

String JSON::getString() {
  String strVal = "";
  String subStr = getItem();
  int start = -1;
  int len = -1;
  start = subStr.indexOf('"') + 1;
  if (start > 0) {
    len = subStr.substring(start).indexOf('"');
  }
  if ((start > 0) && (len != -1)) {
    strVal = subStr.substring(start, start + len);
  }
  return strVal;
}

int JSON::getInt() {
  return getItem().toInt();
}

float JSON::getFloat() {
  return getItem().toFloat();
}

bool JSON::getBoolean() {
  bool boolVal = true;
  String item = getItem();
  if ((item == "false") || (item == "0")) {
    boolVal = false;
  }
  return boolVal;
}

JSON JSON::getJsonItem() {
  JSON json;
  String item = getItem();
  json.LoadJson(item);
  return json;
}

int JSON::getReserved() {
  return reserved;
}

///////////// PRIVATES ///////////////////////////

void JSON::AddToJsonString(String &result) {
  if ((JsonString.length() == 0) || (baseType != JT_JSON)) {
    JsonString = result;
  } else {
    JsonString += ',';
    JsonString += result;
  }
}

int JSON::getJsonBaseType() {
  baseType = JT_UNKNOWN;
  if (JsonString.length() > 0) {
    if (JsonString[0] == '{') {
      baseType = JT_JSON;
    } else if (JsonString[0] == '[') {
      baseType = JT_ARRAY;
    } else if (JsonString[0] == '"') {
      baseType = JT_STRING;
    }
  }
  return baseType;
}

int JSON::getItemType(String &item) {
  int type = JT_UNKNOWN;
  if (item.length() > 0) {
    if (item[0] == '{') {
      type = JT_JSON;
    } else if (item[0] == '[') {
      type = JT_ARRAY;
    } else if (item[0] == '"') {
      type = JT_STRING;
    } else if ((item == "true") || (item == "false")) {
      type = JT_BOOLEAN;
    } else if (item.indexOf('.') != -1) { // bit dangerous, but don't want to test for valid number
      type = JT_FLOAT;
    } else { // bit dangerous, but don't want to test for valid number
      type = JT_INT;
    }
  }
  return type;
}

String JSON::getItem() {
  String item = "";
  int start = -1;
  int end = -1;
  start = findStartPos();
  if (start != -1) {
    end = findEndPos(start);
  }
  if ((start != -1) && (end != -1)) {
    item = JsonString.substring(start + 1, end);
    item.trim();
  }
  return item;
}

int JSON::findStartPos() {
  int start = -1;
  if (baseType == JT_JSON) {
    start = JsonString.indexOf('{');
    if (start != -1) {
      start = JsonString.substring(start + 1).indexOf('"') + start + 1;
    }
    if (start != -1) {
      start = JsonString.substring(start + 1).indexOf('"') + start + 1;
    }
    if (start != -1) {
      start = JsonString.substring(start + 1).indexOf(':') + start + 1;
    }
  } else if (baseType == JT_ARRAY) {
    start = JsonString.indexOf('[');
  } // no item if baseType == JT_STRING
  return start;
}

int JSON::findEndPos(int start) {
  char endChar = '}';
  int pos = 0;
  bool endFound = false;
  int inArray = 0;
  int inJson = 0;
  bool inString = false;
  if (baseType == JT_ARRAY) {
    endChar = ']';
  }	
  pos = start;		
  while (!endFound) {				
	  if ((inArray == 0) && (inJson == 0) && (!inString)) {
      if ((JsonString[pos] == ',') || (JsonString[pos] == endChar)) {		
			  endFound = true;	
      } else if (JsonString[pos] == '[') {		
			  inArray++;	
		  } else if (JsonString[pos] == '{') {		
			  inJson++;	
      } else if (JsonString[pos] == '"') {		
			  inString = true;
      }
    } else if (inArray > 0) { //don't check for json in arrays, assume correct syntax			
      if (inString) {		
        if (JsonString[pos] == '"') {	
          inString = false;
        }
      } else {		
        if (JsonString[pos] == '"') {	
          inString = true;
        } else if (JsonString[pos] == '[') {	
          inArray++;
        } else if (JsonString[pos] == ']') {	
          inArray--;
        }
      }
    } else if (inJson > 0) { //don't check for arrays in json, assume correct syntax			
      if (inString) {		
        if (JsonString[pos] == '"') {	
          inString = false;
        }
		  } else {		
        if (JsonString[pos] == '"') {	
          inString = true;
        } else if (JsonString[pos] == '{') {
          inJson++;
        } else if (JsonString[pos] == '}') {	
          inJson--;
        }
      }
    } else if (inString) { //don't care for other characters in string, do not allow " or \" within string			
      if (JsonString[pos] == '"') {		
        inString = false;	
      }
    }
    if (pos > JsonString.length() - 1) {
      endFound = true;
      pos = -1;
	  } else if (!endFound) {			
		  pos++;
    }
  }
  return pos;				
}