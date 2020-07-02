/*

File WebConfig.h
Version 1.3
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

const char * inputtypes[] = {"text","password","number","date","time","range","check","radio","select","color","float"};

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

//Template for save button and end of the form with save
const char HTML_END[] =
"<div class='zeile'><button type='submit' name='SAVE'>Save</button>\n"
"<button type='submit' name='RST'>Restart</button></div>\n"
"</form>\n"
"</div>\n"
"</body>\n"
"</html>\n";
//Template for save button and end of the form without save
const char HTML_BUTTON[] =
"<button type='submit' name='%s'>%s</button>\n";

WebConfig::WebConfig() {
  _apName = "";
};

void WebConfig::setDescription(String parameter){
  _count = 0;
  addDescription(parameter);
}

void WebConfig::addDescription(String parameter){
  DeserializationError error;
  const int capacity = JSON_ARRAY_SIZE(MAXVALUES)
  + MAXVALUES*JSON_OBJECT_SIZE(6);
  StaticJsonDocument<capacity> doc;
  char tmp[40];
  error = deserializeJson(doc,parameter);
  if (error ) {
    Serial.println(parameter);
    Serial.print("JSON: ");
    Serial.println(error.c_str());
  } else {
    JsonArray array = doc.as<JsonArray>();
    uint8_t j = 0;
    for (JsonObject obj : array) {
      if (_count<MAXVALUES) {
        _description[_count].optionCnt = 0;
        if (obj.containsKey("name")) strlcpy(_description[_count].name,obj["name"],NAMELENGTH);
        if (obj.containsKey("label"))strlcpy(_description[_count].label,obj["label"],LABELLENGTH);
        if (obj.containsKey("type")) {
          if (obj["type"].is<char *>()) {
            uint8_t t = 0;
            strlcpy(tmp,obj["type"],30);
            while ((t<INPUTTYPES) && (strcmp(tmp,inputtypes[t])!=0)) t++;
            if (t>INPUTTYPES) t = 0;
            _description[_count].type = t;
          } else {
            _description[_count].type = obj["type"];
          }
        } else {
          _description[_count].type = INPUTTEXT;
        }
        _description[_count].max = (obj.containsKey("max"))?obj["max"] :100;
        _description[_count].min = (obj.containsKey("min"))?obj["min"] : 0;
        if (obj.containsKey("default")) {
          strlcpy(tmp,obj["default"],30);
          values[_count] = String(tmp);
        } else {
          values[_count]="0";
        }
        if (obj.containsKey("options")) {
          JsonArray opt = obj["options"].as<JsonArray>();
          j = 0;
          for (JsonObject o : opt) {
            if (j<MAXOPTIONS) {
              _description[_count].options[j] = o["v"].as<String>();
              _description[_count].labels[j] = o["l"].as<String>();
            }
            j++;
          }
          _description[_count].optionCnt = opt.size();
        }
      }
      _count++;
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
  boolean exit = false;
  if (server->hasArg("SAVE") && _onSave) {
    _onSave(getResults());
    exit = true;
  }
  if (server->hasArg("DONE") && _onDone) {
    _onDone(getResults());
    exit = true;
  }
  if (server->hasArg("CANCEL") && _onCancel) {
    _onCancel();
    exit = true;
  }
  if (server->hasArg("DELETE") && _onDelete) {
    _onDelete(_apName);
    exit = true;
  }
  if (!exit) {
    server->setContentLength(CONTENT_LENGTH_UNKNOWN);
    sprintf(_buf,HTML_START,_apName.c_str());
    server->send(200, "text/html", _buf);
    if (_buttons == BTN_CONFIG) {
      createSimple(_buf,"apName","Name des Accesspoints","text",_apName);
      server->sendContent(_buf);
    }

    for (uint8_t i = 0; i<_count; i++) {
      switch (_description[i].type) {
        case INPUTFLOAT:
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
    if (_buttons == BTN_CONFIG) {
      server->sendContent(HTML_END);
    } else {
      server->sendContent("<div class='zeile'>\n");
      if ((_buttons & BTN_DONE) == BTN_DONE) {
        sprintf(_buf,HTML_BUTTON,"DONE","Done");
        server->sendContent(_buf);
      }
      if ((_buttons & BTN_CANCEL) == BTN_CANCEL) {
        sprintf(_buf,HTML_BUTTON,"CANCEL","Cancel");
        server->sendContent(_buf);
      }
      if ((_buttons & BTN_DELETE) == BTN_DELETE) {
        sprintf(_buf,HTML_BUTTON,"DELETE","Delete");
        server->sendContent(_buf);
      }
      server->sendContent("</div></form></div></body></html>\n");
    }
  }
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
  File f = SPIFFS.open(filename,"r");
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
  File f = SPIFFS.open(filename,"w");
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


//Get results as a JSON string
String WebConfig::getResults(){
  char buffer[1024];
  StaticJsonDocument<1000> doc;
  for (uint8_t i = 0; i<_count; i++) {
    switch (_description[i].type) {
      case INPUTPASSWORD :
      case INPUTSELECT :
      case INPUTDATE :
      case INPUTTIME :
      case INPUTRADIO :
      case INPUTCOLOR :
      case INPUTTEXT : doc[_description[i].name] = values[i]; break;
      case INPUTCHECKBOX :
      case INPUTRANGE :
      case INPUTNUMBER : doc[_description[i].name] = values[i].toInt(); break;
      case INPUTFLOAT : doc[_description[i].name] = values[i].toFloat(); break;

    }
  }
  serializeJson(doc,buffer);
  return String(buffer);
}

//Ser values from a JSON string
void WebConfig::setValues(String json){
  int val;
  float fval;
  char sval[255];
  DeserializationError error;
  StaticJsonDocument<1000> doc;
  error = deserializeJson(doc, json);
  if (error ) {
    Serial.print("JSON: ");
    Serial.println(error.c_str());
  } else {
    for (uint8_t i = 0; i<_count; i++) {
      if (doc.containsKey(_description[i].name)){
        switch (_description[i].type) {
          case INPUTPASSWORD :
          case INPUTSELECT :
          case INPUTDATE :
          case INPUTTIME :
          case INPUTRADIO :
          case INPUTCOLOR :
          case INPUTTEXT : strlcpy(sval,doc[_description[i].name],255);
            values[i] = String(sval); break;
          case INPUTCHECKBOX :
          case INPUTRANGE :
          case INPUTNUMBER : val = doc[_description[i].name];
            values[i] = String(val); break;
          case INPUTFLOAT : fval = doc[_description[i].name];
            values[i] = String(fval); break;

        }
      }
    }
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

//set the value for a parameter
void WebConfig::setValue(const char*name,String value){
  int16_t i = getIndex(name);
  if (i>=0) values[i] = value;
}

//set the label for a parameter
void WebConfig::setLabel(const char * name, const char* label){
  int16_t i = getIndex(name);
  if (i>=0) strlcpy(_description[i].label,label,LABELLENGTH);
}

//remove all options
void WebConfig::clearOptions(uint8_t index){
  if (index < _count) _description[index].optionCnt = 0;
}

void WebConfig::clearOptions(const char * name){
  int16_t i = getIndex(name);
  if (i >= 0) clearOptions(i);
}

//add a new option
void WebConfig::addOption(uint8_t index, String option){
  addOption(index,option,option);
}

void WebConfig::addOption(uint8_t index, String option, String label){
  if (index < _count) {
    if (_description[index].optionCnt < MAXOPTIONS) {
      _description[index].options[_description[index].optionCnt]=option;
      _description[index].labels[_description[index].optionCnt]=label;
      _description[index].optionCnt++;
    }
  }
}

//modify an option
void WebConfig::setOption(uint8_t index, uint8_t option_index, String option, String label){
  if (index < _count) {
    if (option_index < _description[index].optionCnt) {
      _description[index].options[option_index] = option;
      _description[index].labels[option_index] = label;
    }
  }

}

void WebConfig::setOption(char * name, uint8_t option_index, String option, String label){
  int16_t i = getIndex(name);
  if (i >= 0) setOption(i,option_index,option,label);
}

//get the options count
uint8_t WebConfig::getOptionCount(uint8_t index){
  if (index < _count) {
    return _description[index].optionCnt;
  } else {
    return 0;
  }
}

uint8_t WebConfig::getOptionCount(char * name){
  int16_t i = getIndex(name);
  if (i >= 0) {
    return getOptionCount(i);
  } else {
    return 0;
  }

}

//set form type to doen cancel
void WebConfig::setButtons(uint8_t buttons){
  _buttons = buttons;
}
//register onSave callback
void WebConfig::registerOnSave(void (*callback)(String results)){
  _onSave = callback;
}
//register onSave callback
void WebConfig::registerOnDone(void (*callback)(String results)){
  _onDone = callback;
}
//register onSave callback
void WebConfig::registerOnCancel(void (*callback)()){
  _onCancel = callback;
}
//register onDelete callback
void WebConfig::registerOnDelete(void (*callback)(String name)){
  _onDelete = callback;
}
