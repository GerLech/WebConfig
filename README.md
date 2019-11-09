# WebConfig

This Arduino Library works with ESP8266 and ESP32

It allows to edit configuration data by a web form. The configuration data will be storde in the SPIFFS and can be reloades from there.
The web form can be free formatted using JSON.
If you start an AccessPoint, you can use WebConfig to edit access information for the local Network

## Constants:

maximum number of parameters  
**MAXVALUES 20**  

maximum number of options per parameters  
**MAXOPTIONS 15**  

name for the config file  
**CONFFILE "/WebConf.conf"**  

Type of HTML input fields
- INPUTTEXT 0       Simple text input
- INPUTPASSWORD 1   Password input showing stars
- INPUTNUMBER 2     Numeric input
- INPUTDATE 3       Date input
- INPUTTIME 4       Time input
- INPUTRANGE 5      Slider
- INPUTCHECKBOX 6   Checkbox
- INPUTRADIO 7      Radio buttons
- INPUTSELECT 8     Drop down list
- INPUTCOLOR 9      Color picker

## Functions
load form descriptions create the internal structure from JSON String  
**void setDescription(String parameter);**

function to respond a HTTP request for the form use the filename
to save.  
**void handleFormRequest(WebServer * server, const char * filename);**

function to respond a HTTP request for the form use the default filename
to save  
**void handleFormRequest(WebServer * server);**

get the index for a value by parameter name  
**int16_t getIndex(const char * name);**

read configuration from file with filename  
**boolean readConfig(const char *  filename);**

read configuration from file with default filename  
**boolean readConfig();**

write configuration to file with filename  
**boolean writeConfig(const char *  filename);**

write configuration to file with default filename  
**boolean writeConfig();**

delete configuration  file with filename  
**boolean deleteConfig(const char *  filename);**

delete configutation file with default filename  
**boolean deleteConfig();**

get a parameter value as String by its name  
**const String getString(const char * name);**

get a parameter value as pointer to character array by its name  
**const char * getValue(const char * name);**

get a parameter value as integer by its name  
**const int getInt(const char * name);**

get a parameter value as floating point number by its name  
**const float getFloat(const char * name);**

get a parameter value as boolean by its name  
**const boolean getBool(const char * name);**

get the accesspoint name  
**const char * getApName();**

get the number of parameters  
**uint8_t getCount();**

get the name of the parameter with index  
**String getName(uint8_t index);**

direkt access to the parameter values  
**String values\[MAXVALUES\];**

## Parameter definition with JSON

\[{  
"name":"",  
"label":"",  
"type":0,  
"default":""  
"min":0,  
"max":0,  
"options":\[  
  {"v":"","":""}  
\]    
}\]  

**name** String  
 The name of the Parameter. This name will be used to save the parameter in the configuration file. It is also used to access the values.  
 
**label** String  
Defines the label for the web form

**type** Integer  
Type of the HTML input element
-	INPUTTEXT Texteingabefeld
-	INPUTPASSWORD Passwort Eingabefeld
-	INPUTNUMBER Nummern Eingabefeld
-	INPUTDATE Datums Eingabefeld
-	INPUTTIME Zeit Eingabefeld
-	INPUTRANGE Slider zur Nummerneingabe
-	INPUTCHECKBOX Ja/Nein Auswahl
-	INPUTRADIO Mehrfachauswahl
-	INPUTSELECT Mehrfachauswahl aus Dropdown-Liste
-	INPUTCOLOR Farbauswahl

**default**	String  
default value  

**min** Integer	(optional)  
Minimum value for number input  

**max**	    Integer	(optional)  
Maximum value for number input  

**options**	List of objects (optional)  
A list to define options and values for multi select input fields  


#### Example defines a JSON String with all types of input fields  

String params = "\["  
  "{"  
  "'name':'ssid',"  
  "'label':'WLAN name',"  
  "'type':"+String(INPUTTEXT)+","  
  "'default':''"  
  "},"  
  "{"  
  "'name':'pwd',"  
  "'label':'WLAN Password',"  
  "'type':"+String(INPUTPASSWORD)+","  
  "'default':''"  
  "},"  
  "{"  
  "'name':'amount',"  
  "'label':'Amount',"  
  "'type':"+String(INPUTNUMBER)+","  
  "'min':-10,'max':20,"  
  "'default':'1'"  
  "},"  
  "{"  
  "'name':'float',"  
  "'label':'Floatingpoint number',"  
  "'type':"+String(INPUTTEXT)+","  
  "'default':'1.00'"  
  "},"  
  "{"  
  "'name':'duration',"  
  "'label':'Duration (s)',"  
  "'type':"+String(INPUTRANGE)+","  
  "'min':5,'max':30,"  
  "'default':'10'"  
  "},"  
  "{"  
  "'name':'date',"  
  "'label':'Date',"  
  "'type':"+String(INPUTDATE)+","  
  "'default':'2019-08-14'"  
  "},"  
  "{"  
  "'name':'time',"  
  "'label':'Time',"  
  "'type':"+String(INPUTTIME)+","  
  "'default':'18:30'"  
  "},"  
  "{"  
  "'name':'col',"  
  "'label':'Color',"  
  "'type':"+String(INPUTCOLOR)+","  
  "'default':'#ffffff'"  
  "},"  
  "{"  
  "'name':'switch',"  
  "'label':'Switch',"  
  "'type':"+String(INPUTCHECKBOX)+","  
  "'default':'1'"  
  "},"  
  "{"  
  "'name':'gender',"  
  "'label':'Gender',"  
  "'type':"+String(INPUTRADIO)+","  
  "'options':\["  
  "{'v':'m','l':'male'},"  
  "{'v':'f','l':'female'},"  
  "{'v':'o','l':'other'}\],"  
  "'default':'f'"  
  "},"  
  "{"  
  "'name':'continent',"  
  "'label':'Continent',"  
  "'type':"+String(INPUTSELECT)+","  
  "'options':\["  
  "{'v':'EU','l':'Europe'},"  
  "{'v':'AF','l':'Africa'},"  
  "{'v':'AS','l':'Asia'},"  
  "{'v':'AU','l':'Australia'},"  
  "{'v':'AM','l':'America'}\],"  
  "'default':'AM'"  
  "}"  
  "\]";  
  
