//Header Files
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

//Wifi Connection of ESP8266
const char* ssid = "Chirag's S22";
const char* password = "hahaha123";

//API Fields of ThingSpeak Channel
const char* server = "http://api.thingspeak.com";
const char* apikey = "LGRYWV2SCLMI41TC

//LED State Variable
bool ledState = false;

//WiFi Client Object for HTTP Request
WiFiClient client;

void setup()
{
  //Assign GPIO5 or D1 to LED
  pinMode(5, OUTPUT);

  //Connecting to Wifi
  Serial.begin(115200);
  Serial.print("Connecting to Wifi");
  Serial.println(ssid);
  WiFi.begin(ssid,password);
  Serial.print("Connecting");
  while(WiFi.status()!=WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.print("\nWifi Connected\nNodeMCU IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  //Inverting State of LED
  ledState = !ledState;
  digitalWrite(5, ledState ? HIGH : LOW);
  Serial.print("LED is ");
  if (ledState) Serial.println("On");
  else Serial.println("Off");

  //HTTP Request to Send Status Update to ThingSpeak
  if (WiFi.status()==WL_CONNECTED)
  {
    HTTPClient http;
    String url = String(server)+"/update?api_key="+String(apikey)+"&field1="+(ledState?"1":"0");
    http.begin(client, url);
    int httpCode = http.GET();
    Serial.println("HTTP Response code: "+String(httpCode));
    http.end();
  }
  delay(15000);
}
