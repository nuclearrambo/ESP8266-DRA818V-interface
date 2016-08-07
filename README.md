# ESP8266-DRA818V-interface
ESP8266 firmware to interface with the DRA818V module. 

## Functioning
Connects to a WiFi AP. Starts a Webserver that has a html form which accepts the Frequency that is to be set on the DRA818V module. 
Form action is POST. Therefore, by directly "POST"ing the parameters will tune the DRA818V module.

### Parameters: 
"frequency"
"squelch"

Android App does a "Httppost" containing these parameters. The ESP8266 uses these values and inserts them in the DRA818V AT command string.
The command string is then sent to the DRA818 module over UART. 
The response is recorded and posted back as the HTTP response to the client. 
