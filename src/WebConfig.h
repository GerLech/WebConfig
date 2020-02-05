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
#ifndef WebConfig_h
#define WebConfig_h

#include <Arduino.h>
#if defined(ESP32)
  #include <WebServer.h>
#else
  #include <ESP8266WebServer.h>
#endif

//maximum number of parameters
#define MAXVALUES 20

//maximum number of options per parameters
#define MAXOPTIONS 15

//character limits
#define NAMELENGTH 20
#define LABELLENGTH 40

//name for the config file
#define CONFFILE "/WebConf.conf"

#define INPUTTEXT 0
#define INPUTPASSWORD 1
#define INPUTNUMBER 2
#define INPUTDATE 3
#define INPUTTIME 4
#define INPUTRANGE 5
#define INPUTCHECKBOX 6
#define INPUTRADIO 7
#define INPUTSELECT 8
#define INPUTCOLOR 9
#define INPUTFLOAT 10
//number of types
#define INPUTTYPES 11

//data structure to hold the parameter Description
typedef //Struktur eines Datenpakets
struct  {
  char name[NAMELENGTH];
  char label[LABELLENGTH];
  uint8_t type;
  int min;
  int max;
  uint8_t optionCnt;
  String options[MAXOPTIONS];
  String labels[MAXOPTIONS];
} DESCRIPTION;

class WebConfig {
public:
  WebConfig();
  //load form descriptions
  void setDescription(String parameter);
  //Add extra descriptions
  void addDescription(String parameter);
  //function to respond a HTTP request for the form use the filename
  //to save.
#if defined(ESP32)
  void handleFormRequest(WebServer * server, const char * filename);
  //function to respond a HTTP request for the form use the default file
  //to save and restart ESP after saving the new config
  void handleFormRequest(WebServer * server);
  //get the index for a value by parameter name
#else
  void handleFormRequest(ESP8266WebServer * server, const char * filename);
  //function to respond a HTTP request for the form use the default file
  //to save and restart ESP after saving the new config
  void handleFormRequest(ESP8266WebServer * server);
  //get the index for a value by parameter name
#endif
  int16_t getIndex(const char * name);
  //read configuration from file
  boolean readConfig(const char *  filename);
  //read configuration from default file
  boolean readConfig();
  //write configuration to file
  boolean writeConfig(const char *  filename);
  //write configuration to default file
  boolean writeConfig();
  //delete configuration file
  boolean deleteConfig(const char *  filename);
  //delete default configutation file
  boolean deleteConfig();
  //get a parameter value by its name
  const String getString(const char * name);
  const char * getValue(const char * name);
  const int getInt(const char * name);
  const float getFloat(const char * name);
  const boolean getBool(const char * name);
  //get the accesspoint name
  const char * getApName();
  //get the number of parameters
  uint8_t getCount();
  //get the name of a parameter
  String getName(uint8_t index);
  //Get results as a JSON string
  String getResults();
  //set the value for a parameter
  void setValue(const char*name,String value);
  //set the label for a parameter
  void setLabel(const char * name, const char* label);
  //remove all options
  void clearOptions(uint8_t index);
  void clearOptions(const char * name);
  //add a new option
  void addOption(uint8_t index, String option);
  void addOption(uint8_t index, String option, String label);
  //modify an option
  void setOption(uint8_t index, uint8_t option_index, String option, String label);
  void setOption(char * name, uint8_t option_index, String option, String label);
  //get the options count
  uint8_t getOptionCount(uint8_t index);
  uint8_t getOptionCount(char * name);
  //values for the parameter
  String values[MAXVALUES];
private:
  char _buf[1000];
  uint8_t _count;
  String _apName;
  DESCRIPTION _description[MAXVALUES];
};

#endif
