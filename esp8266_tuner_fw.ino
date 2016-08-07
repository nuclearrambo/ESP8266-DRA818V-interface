#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "HAM_NODE";
const char* password = "";
String frequency = String("");
String squelch = String("");
ESP8266WebServer server(80);
void writeString(String string);
float mapFl(int x, float in_min, float in_max, float out_min, float out_max);
const int led = 13;

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/html", "<!DOCTYPE HTML><html><head><title>Ham tuner</title></head><body><form class=\"form-horizontal\" action=\"/submit\" method=\"post\">"
"<fieldset>"

"<!-- Form Name -->"
"<legend>Form Name</legend>"

"<!-- Text input-->"
"<div class=\"form-group\">"
  "<label class=\"col-md-4 control-label\" for=\"frequency\">Frequency</label> " 
  "<div class=\"col-md-4\">"
  "<input id=\"frequency\" name=\"frequency\" placeholder=\"e.g. 145.6000\" class=\"form-control input-md\" type=\"text\">"
  "<span class=\"help-block\">Enter frequency</span> " 
 " </div>"
"</div>"

"<!-- Text input-->"
"<div class=\"form-group\">"
  "<label class=\"col-md-4 control-label\" for=\"squelch\">Squelch level</label>  "
  "<div class=\"col-md-4\">"
  "<input id=\"squelch\" name=\"squelch\" placeholder=\"e.g. 1\" class=\"form-control input-md\" type=\"text\">"
  "<span class=\"help-block\">Enter squelch level between 0 to 7</span>  "
  "</div>"
"</div>"
"<!-- Button -->"
"<div class=\"form-group\">"
  "<label class=\"col-md-4 control-label\" for=\"submit\"></label>"
  "<div class=\"col-md-4\">"
    "<button id=\"submit\" name=\"submit\" class=\"btn btn-primary\">Tune</button>"
  "</div>"
"</div>"

"</fieldset>"
"</form></body></html>"
);
  digitalWrite(led, 0);
}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void handleSubmit(){
  float floatFrequency;
  char buffer[100];
  if(server.args()>0){
    if(server.hasArg("frequency")){
      frequency = server.arg("frequency");
      squelch = server.arg("squelch");

      //floatFrequency = frequency.toFloat();
      //floatFrequency = mapFl(floatFrequency, 1, 100, 144.000, 146.000);   
      //dtostrf(floatFrequency, 3, 3, buffer);
      //Serial.println(buffer);

      writeString("AT+DMOSETGROUP=1,"+frequency+","+frequency+",0000,"+squelch+",0000\r\n");
                   
    }
  }
  digitalWrite(led, 1);
  String message;
  String rx;
  delay(100);
  if(Serial.available()>0){
    rx = Serial.readString();
  }
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  message += rx + "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  digitalWrite(led, 0);
  server.send(200, "text/html", message);
}

void setup(void){
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(9600);
  IPAddress ip(192,168,1,35);
  IPAddress subnet(255,255,255,0);
  
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(ip, ip, subnet);
  Serial.println("");

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/submit", handleSubmit);
  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}

void writeString(String string) { // Used to serially push out a String with Serial.write()

  for (int i = 0; i < string.length(); i++)
  {
    Serial.write(string[i]);   // Push each char 1 by 1 on each loop pass
  }

}

float mapFl(int x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
