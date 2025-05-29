#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "Chirag's S22";
const char* password = "hahaha123";
const char* server = "http://api.thingspeak.com";
const char* apikey = "LGRYWV2SCLMI41TC";
bool ledState = false;
WiFiClient client;

void setup() {
  // put your setup code here, to run once:
  pinMode(5, OUTPUT);
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
  // put your main code here, to run repeatedly:
  ledState = !ledState;
  digitalWrite(5, ledState ? HIGH : LOW);
  Serial.print("LED is ");
  if (ledState) Serial.println("On");
  else Serial.println("Off");
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
