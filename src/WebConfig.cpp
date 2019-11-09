/*

File WebConfig.h
Version 0.1
Author Gerald Lechner
contakt lechge@gmail.com

Description
This library builds a web page with a smart phone friendly form to edit
a free definable number of configuration parameters.
The submitted data will bestored in the SPIFFS
The library works with ESP8266 and ESP32

Dependencies:
  ESP8266WebServer.h
  ArduinoJson.h

*/
#include <WebConfig.h>
#include <Arduino.h>
#if defined(ESP32)
  #include "SPIFFS.h"
  #include <WebServer.h>
#else
  #include <ESP8266Webserver.h>
#endif
#include <ArduinoJson.h>
#include <FS.h>

//HTML templates
//Template for header and begin of form
const char HTML_START[] =
"<!DOCTYPE HTML>\n"
"<html lang='de'>\n"
"<head>\n"
"<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>\n"
"<meta name='viewport' content='width=320' />\n"
"<title>Konfiguration</title>\n"
"<style>\n"
"body {\n"
"  background-color: #d2f3eb;\n"
"  font-family: Arial, Helvetica, Sans-Serif;\n"
"  Color: #000000;\n"
"  font-size:12pt;\n"
"  width:320px;\n"
"}\n"
".titel {\n"
"font-weight:bold;\n"
"text-align:center;\n"
"width:100%;\n"
"padding:5px;\n"
"}\n"
".zeile {\n"
"  width:100%;\n"
"  padding:5px;\n"
"  text-align: center;\n"
"}\n"
"button {\n"
"font-size:14pt;\n"
"width:150px;\n"
"border-radius:10px;\n"
"margin:5px;\n"
"}\n"
"</style>\n"
"</head>\n"
"<body>\n"
"<div id='main_div' style='margin-left:15px;margin-right:15px;'>\n"
"<div class='titel'>Konfiguration %s</div>\n"
"<form method='post'>\n";

//Template for one input field
const char HTML_ENTRY_SIMPLE[] =
"  <div class='zeile'><b>%s</b></div>\n"
"  <div class='zeile'><input type='%s' value='%s' name='%s' \></div>\n";
const char HTML_ENTRY_NUMBER[] =
"  <div class='zeile'><b>%s</b></div>\n"
"  <div class='zeile'><input type='number' min='%i' max='%i' value='%s' name='%s' \></div>\n";
const char HTML_ENTRY_RANGE[] =
"  <div class='zeile'><b>%s</b></div>\n"
"  <div class='zeile'>%i&nbsp;<input type='range' min='%i' max='%i' value='%s' name='%s' \>&nbsp;%i</div>\n";
const char HTML_ENTRY_CHECKBOX[] =
"  <div class='zeile'><b>%s</b><input type='checkbox' %s name='%s' \></div>\n";
const char HTML_ENTRY_RADIO_TITLE[] =
" <div class='zeile'><b>%s</b></div>\n";
const char HTML_ENTRY_RADIO[] =
"  <div class='zeile'><input type='radio' name='%s' value='%s' %s>%s</div>\n";
const char HTML_ENTRY_SELECT_START[] =
" <div class='zeile'><b>%s</b></div>\n"
" <div class='zeile'><select name='%s'>\n";
const char HTML_ENTRY_SELECT_OPTION[] =
"  <option value='%s' %s>%s</option>\n";
const char HTML_ENTRY_SELECT_END[] =
" </select></div>\n";

//Template for save button and end of the form
const char HTML_END[] =
"<div class='zeile'><button type='submit' name='SAVE'>Save</button>\n"
"<button type='submit' name='RST'>Restart</button></div>\n"
"</form>\n"
"</div>\n"
"</body>\n"
"</html>\n";

WebConfig::WebConfig() {
  _apName = "";
};

void WebConfig::setDescription(String parameter){
  DeserializationError error;
  const int capacity = JSON_ARRAY_SIZE(MAXVALUES)
  + MAXVALUES*JSON_OBJECT_SIZE(6);
  StaticJsonDocument<capacity> doc;
  char tmp[40];
  error = deserializeJson(doc,parameter);
  if (error ) {
    Serial.print("JSON: ");
    Serial.println(error.c_str());
  } else {
    JsonArray array = doc.as<JsonArray>();
    _count = array.size();
    uint8_t i = 0;
    uint8_t j = 0;
    for (JsonObject obj : array) {
      if (i<MAXVALUES) {
        _description[i].optionCnt = 0;
        strlcpy(_description[i].name,obj["name"],15);
        strlcpy(_description[i].label,obj["label"],40);
        _description[i].type = (obj.containsKey("type"))?obj["type"]:INPUTTEXT;
        _description[i].max = (obj.containsKey("max"))?obj["max"] :100;
        _description[i].min = (obj.containsKey("min"))?obj["min"] : 0;
        strlcpy(tmp,obj["default"],30);
        values[i] = String(tmp);
        if (obj.containsKey("options")) {
          JsonArray opt = obj["options"].as<JsonArray>();
          j = 0;
          for (JsonObject o : opt) {
            if (j<MAXOPTIONS) {
              _description[i].options[j] = o["v"].as<String>();
              _description[i].labels[j] = o["l"].as<String>();
            }
            j++;
          }
          _description[i].optionCnt = opt.size();
        }
      }
      i++;
    }

  }
  _apName = WiFi.macAddress();
  _apName.replace(":","");
  SPIFFS.begin();
};

void createSimple(char * buf, char * name, char * label, char * type, String value) {
  sprintf(buf,HTML_ENTRY_SIMPLE,label,type,value.c_str(),name);
}

void createNumber(char * buf, DESCRIPTION descr, String value) {
  sprintf(buf,HTML_ENTRY_NUMBER,descr.label,descr.min,descr.max, value.c_str(),descr.name);
}

void createRange(char * buf, DESCRIPTION descr, String value) {
  sprintf(buf,HTML_ENTRY_RANGE,descr.label,descr.min,descr.min,descr.max,value.c_str(),descr.name,descr.max);
}

void createCheckbox(char * buf , DESCRIPTION descr, String value) {
  if (value != "0") {
    sprintf(buf,HTML_ENTRY_CHECKBOX,descr.label,"checked",descr.name);
  } else {
    sprintf(buf,HTML_ENTRY_CHECKBOX,descr.label,"",descr.name);
  }
}

void createRadio(char * buf , DESCRIPTION descr, String value, uint8_t index) {
  if (value == descr.options[index]) {
    sprintf(buf,HTML_ENTRY_RADIO,descr.name,descr.options[index].c_str(),"checked",descr.labels[index].c_str());
  } else {
    sprintf(buf,HTML_ENTRY_RADIO,descr.name,descr.options[index].c_str(),"",descr.labels[index].c_str());
  }
}

void startSelect(char * buf , DESCRIPTION descr) {
  sprintf(buf,HTML_ENTRY_SELECT_START,descr.label,descr.name);
}

void addSelectOption(char * buf, String option, String label, String value) {
  if (option == value) {
    sprintf(buf,HTML_ENTRY_SELECT_OPTION,option.c_str(),"selected",label.c_str());
  } else {
    sprintf(buf,HTML_ENTRY_SELECT_OPTION,option.c_str(),"",label.c_str());
  }
}

//***********Different type for ESP32 WebServer and ESP8266WebServer ********
//both classes have the same functions
#if defined(ESP32)
  //function to respond a HTTP request for the form use the default file
  //to save and restart ESP after saving the new config
  void WebConfig::handleFormRequest(WebServer * server){
    handleFormRequest(server,CONFFILE);
  }
  //function to respond a HTTP request for the form use the filename
  //to save. If auto is true restart ESP after saving the new config
  void WebConfig::handleFormRequest(WebServer * server, const char * filename){
#else
  //function to respond a HTTP request for the form use the default file
  //to save and restart ESP after saving the new config
  void WebConfig::handleFormRequest(ESP8266WebServer * server){
    handleFormRequest(server,CONFFILE);
  }
  //function to respond a HTTP request for the form use the filename
  //to save. If auto is true restart ESP after saving the new config
  void WebConfig::handleFormRequest(ESP8266WebServer * server, const char * filename){
#endif
//******************** Rest of the function has no difference ***************
  if (server->args() > 0) {
    if (server->hasArg("apName")) _apName = server->arg("apName");
    for (uint8_t i= 0; i < _count; i++) {
      if (_description[i].type == INPUTCHECKBOX) {
        values[i] = "0";
        if (server->hasArg(_description[i].name)) values[i] = "1";
      } else {
        if (server->hasArg(_description[i].name)) values[i] = server->arg(_description[i].name) ;
      }
    }
    if (server->hasArg("SAVE") || server->hasArg("RST")) {
      writeConfig(filename);
      if (server->hasArg("RST")) ESP.restart();
    }
  }
  server->setContentLength(CONTENT_LENGTH_UNKNOWN);
  sprintf(_buf,HTML_START,_apName.c_str());
  server->send(200, "text/html", _buf);
  createSimple(_buf,"apName","Name des Accesspoints","text",_apName);
  server->sendContent(_buf);

  for (uint8_t i = 0; i<_count; i++) {
    switch (_description[i].type) {
      case INPUTTEXT: createSimple(_buf,_description[i].name,_description[i].label,"text",values[i]);
        break;
      case INPUTPASSWORD: createSimple(_buf,_description[i].name,_description[i].label,"password",values[i]);
        break;
      case INPUTDATE: createSimple(_buf,_description[i].name,_description[i].label,"date",values[i]);
        break;
      case INPUTTIME: createSimple(_buf,_description[i].name,_description[i].label,"time",values[i]);
        break;
      case INPUTCOLOR: createSimple(_buf,_description[i].name,_description[i].label,"color",values[i]);
        break;
      case INPUTNUMBER: createNumber(_buf,_description[i],values[i]);
        break;
      case INPUTRANGE: createRange(_buf,_description[i],values[i]);
        break;
      case INPUTCHECKBOX: createCheckbox(_buf,_description[i],values[i]);
        break;
      case INPUTRADIO: sprintf(_buf,HTML_ENTRY_RADIO_TITLE,_description[i].label);
        for (uint8_t j = 0 ; j<_description[i].optionCnt; j++) {
          server->sendContent(_buf);
          createRadio(_buf,_description[i],values[i],j);
        }
        break;
      case INPUTSELECT: startSelect(_buf,_description[i]);
        for (uint8_t j = 0 ; j<_description[i].optionCnt; j++) {
          server->sendContent(_buf);
          addSelectOption(_buf,_description[i].options[j],_description[i].labels[j],values[i]);
        }
        server->sendContent(_buf);
        strcpy(_buf,HTML_ENTRY_SELECT_END);
        break;
      default : _buf[0] = 0;
        break;

    }
    server->sendContent(_buf);
  }

  server->sendContent(HTML_END);
}
//get the index for a value by parameter name
int16_t WebConfig::getIndex(const char * name){
  int16_t i = _count-1;
  while ((i>=0) && (strcmp(name,_description[i].name)!=0)) {
    i--;
  }
  return i;
}
//read configuration from file
boolean WebConfig::readConfig(const char * filename){
  String line,name,value;
  uint8_t pos;
  int16_t index;
  if (!SPIFFS.exists(filename)) {
    //if configfile does not exist write default values
    writeConfig(filename);
  }
  File f = SPIFFS.open(CONFFILE,"r");
  if (f) {
    Serial.println("Read configuration");
    uint32_t size = f.size();
    while (f.position() < size) {
      line = f.readStringUntil(10);
      pos = line.indexOf('=');
      name = line.substring(0,pos);
      value = line.substring(pos+1);
      if (name == "apName") {
        _apName = value;
        Serial.println(line);
      } else {
        index = getIndex(name.c_str());
        if (!(index < 0)) {
          values[index] = value;
          if (_description[index].type == INPUTPASSWORD) {
            Serial.printf("%s=*************\n",_description[index].name);
          } else {
            Serial.println(line);
          }
        }
      }
    }
    f.close();
    return true;
  } else {
    Serial.println("Cannot read configuration");
    return false;
  }
}
//read configuration from default file
boolean WebConfig::readConfig(){
  return readConfig(CONFFILE);
}
//write configuration to file
boolean WebConfig::writeConfig(const char * filename){
  File f = SPIFFS.open(CONFFILE,"w");
  if (f) {
    f.printf("apName=%s\n",_apName.c_str());
    for (uint8_t i = 0; i<_count; i++){
      f.printf("%s=%s\n",_description[i].name,values[i].c_str());
    }
    return true;
  } else {
    Serial.println("Cannot write configuration");
    return false;
  }

}
//write configuration to default file
boolean WebConfig::writeConfig(){
  return writeConfig(CONFFILE);
}
//delete configuration file
boolean WebConfig::deleteConfig(const char * filename){
  return remove(filename);
}
//delete default configutation file
boolean WebConfig::deleteConfig(){
  return deleteConfig(CONFFILE);
}

//get a parameter value by its name
const String WebConfig::getString(const char * name) {
  int16_t index;
  index = getIndex(name);
  if (index < 0) {
    return "";
  } else {
    return values[index];
  }
}

const char * WebConfig::getValue(const char * name){
  int16_t index;
  index = getIndex(name);
  if (index < 0) {
    return "";
  } else {
    return values[index].c_str();
  }
}

const int WebConfig::getInt(const char * name){
  return getString(name).toInt();
}

const float WebConfig::getFloat(const char * name){
  return getString(name).toFloat();
}

const boolean WebConfig::getBool(const char * name){
  return (getString(name) != "0");
}

//get the accesspoint name
const char * WebConfig::getApName(){
  return _apName.c_str();
}
//get the number of parameters
uint8_t WebConfig::getCount(){
  return _count;
}

//get the name of a parameter
String WebConfig::getName(uint8_t index){
  if (index < _count) {
    return String(_description[index].name);
  } else {
    return "";
  }
}
