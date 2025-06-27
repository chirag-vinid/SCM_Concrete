#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define LED_PIN D4  // GPIO2 on ESP8266
bool status = true;

// Replace with your Wi-Fi credentials
const char* ssid = "Airtel_9962583910";
const char* password = "air10719";

// Replace with your laptop's IP address (Flask host)
const char* serverIP = "192.168.1.3";  // <-- change this
const int serverPort = 5000;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void loop() {
  if (status) {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED is ON");
  } else {
    digitalWrite(LED_PIN, LOW);
    Serial.println("LED is OFF");
  }

  // Send POST request to Flask
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://" + String(serverIP) + ":" + String(serverPort) + "/update";
    WiFiClient client;
    http.begin(client, url);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"led_state\": \"" + String(status ? "ON" : "OFF") + "\"}";
    int httpCode = http.POST(payload);

    Serial.print("POST Status Code: ");
    Serial.println(httpCode);

    http.end();
  } else {
    Serial.println("WiFi not connected.");
  }

  delay(5000);
  status = !status;
}
