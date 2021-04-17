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
- INPUTFLOAT 10     Floatingpoint number
- INPUTTEXTAREA 11  Text area with multiple lines
- INPUTMULTICHECK 12 Fieldset with multiple checkboxes. The result string has one character
0 or 1 for every option. 010010 means there are 6 options and option 2 and 5 are true

## Functions

**void setDescription(String parameter);**

load form descriptions create the internal structure from JSON String, delete existing entries

**void addDescription(String parameter);**

 add more form descriptions from JSON String  

 **void handleFormRequest(WebServer * server, const char * filename);**

function to respond a HTTP request for the form use the filename
to save.  

**void handleFormRequest(WebServer * server);**

function to respond a HTTP request for the form use the default filename
to save  

**int16_t getIndex(const char * name);**

get the index for a value by parameter name  

**boolean readConfig(const char *  filename);**

read configuration from file with filename  

**boolean readConfig();**

read configuration from file with default filename  

**boolean writeConfig(const char *  filename);**

write configuration to file with filename  

**boolean writeConfig();**

write configuration to file with default filename  

**boolean deleteConfig(const char *  filename);**

delete configuration  file with filename  

**boolean deleteConfig();**

delete configutation file with default filename  

**const String getString(const char * name);**

get a parameter value as String by its name  

**const char * getValue(const char * name);**

get a parameter value as pointer to character array by its name  

**const int getInt(const char * name);**

get a parameter value as integer by its name  

**const float getFloat(const char * name);**

get a parameter value as floating point number by its name  

**const boolean getBool(const char * name);**

get a parameter value as boolean by its name  

**const char * getApName();**

get the accesspoint name  

**uint8_t getCount();**

get the number of parameters  

**String getName(uint8_t index);**

get the name of the parameter with index  

**String values\[MAXVALUES\];**

direkt access to the parameter values  

**String getResults();**

get the values for all fields in a JSON formatted string

**void setValues(String json);**

preset the values for all fields out of a JSON formatted string

**void setValue(const char * name,String value);**

set the value of the field named name with the value from value

**void setLabel(const char * name, const char* label);**

set the label for the field named name with the value from label

**void clearOptions(uint8_t index);**

remove all options for the selection field with index index

**void clearOptions(const char * name);**

remove all options for the selection field with name name

**void addOption(uint8_t index, String option);**

add an option to the selection field with index index. The value from parameter option is uesd for label and value

**void addOption(uint8_t index, String option, String label);**

add an option to the selection field with index index. The value from parameter option is uesd for the value and the value from parameter label is used for the label

**void setOption(uint8_t index, uint8_t option_index, String option, String label);**

modify an option in the selection field with index index. The option with index option_index gets the value from parameter option and the label from parameter label

**void setOption(char * name, uint8_t option_index, String option, String label);**

modify an option in the selection field with name name. The option with index option_index gets the value from parameter option and the label from parameter label

**uint8_t getOptionCount(uint8_t index);**

returns the number of options in the selection field with index index

**uint8_t getOptionCount(char * name);**

returns the number of options in the selection field with name name

**void setButtons(uint8 buttons);**

set the type of form. With BTN_CONFIG (0) the configuration mode will be set. This form is typical used to setup WiFi access. The form has two buttons "SAVE" and "RESTART". Modifications will be saved to SPIFFS automatically. With BTN_DONE (1), BTN_CANCEL (2) and BTN_DELETE (4) simple forms will be shown, without automatic saving. The form gets the specified Buttons. The buttons can be combined. So BTN_DONE+BTN_CANCEL+BTN_DELETE shows all three buttons. To react on button clicks, callback functions can be registered.

**void registerOnSave(void (\*callback)(String results));**

this function will be called after the "SAVE" button was clicked. The parameter results holds a JSON formatted string with the values from all fields.

**void registerOnDone(void (\*callback)(String results));**

this function will be called after the "DONE" button was clicked. The parameter results holds a JSON formatted string with the values from all fields.

**void registerOnCancel(void (\*callback)());**

this function will be called after the "CANCEL" button was clicked.

**void registerOnDelete(void (\*callback)(String name));**

this function will be called after the "DELETE" button was clicked. The parameter name holds the value of the field named "name" if such a field exists.

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
- INPUTFLOAT Fließkommazahl
- INPUTTEXTAREA Mehrzeiliges Textfeld
- INPUTMULTICHECK Mehrere Checkboxen

**default**	String  
default value  

**min** Integer	(optional)  
Minimum value for number input  or columns for test area

**max**	    Integer	(optional)  
Maximum value for number input  or rows for text area

**options**	List of objects (optional)  
A list to define options and values for multi select input fields  on multi checkboxes the option name is used as label


#### Example defines a JSON String with all types of input fields  

String params = "\["  
  "{"  
  "'name':'ssid',"  
  "'label':'WLAN name',"  
  "'type':"+String(INPUTTEXT)+","  
  "'default':''"  
  "},"  
  "{"
  "'name':'area',"
  "'label':'Textarea',"
  "'type':"+String(INPUTTEXTAREA)+","
  "'default':'',"
  "'min':40,'max':5"  //min = columns max = rows
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
  "},"  
  "{"
"'name':'wochentag',"
"'label':'Wochentag',"
"'type':"+String(INPUTMULTICHECK)+","
"'options':["
"{'v':'0','l':'Sonntag'},"
"{'v':'1','l':'Montag'},"
"{'v':'2','l':'Dienstag'},"
"{'v':'3','l':'Mittwoch'},"
"{'v':'4','l':'Donnerstag'},"
"{'v':'5','l':'Freitag'},"
"{'v':'6','l':'Samstag'}],"
"'default':''"
"}"
"\]";  
