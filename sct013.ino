#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "EmonLib.h"

// Wi-Fi credentials
const char* ssid = "Airtel_9962583910";
const char* password = "air10719";

// Flask server IP
const char* flask_server_ip = "192.168.1.3";
const int flask_port = 5000;

// LED Pin and Blinking
const int ledPin = D4;
unsigned long previousMillisLED = 0;
const long blinkInterval = 500;
bool ledState = LOW;

// SCT-013 Current Sensor
EnergyMonitor emon1;
const int SCT013_ANALOG_PIN = A0;
const float SCT013_CALIBRATION_CONSTANT = 30.0; // Calibrate this value!
const int SCT013_NUM_SAMPLES = 1480; // For 50Hz mains

WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(100);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);

  // Connect to Wi-Fi
  Serial.print("Connecting to WiFi: "); Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected. IP: " + WiFi.localIP().toString());
  digitalWrite(ledPin, LOW);

  // SCT-013 Setup
  emon1.current(SCT013_ANALOG_PIN, SCT013_CALIBRATION_CONSTANT);
  Serial.println("SCT-013 setup complete.");
}

void loop() {
  unsigned long currentMillis = millis();

  // LED blinking
  if (WiFi.status() == WL_CONNECTED) {
    if (currentMillis - previousMillisLED >= blinkInterval) {
      previousMillisLED = currentMillis;
      ledState = !ledState;
      digitalWrite(ledPin, ledState ? HIGH : LOW);
    }
  } else {
    digitalWrite(ledPin, HIGH);
  }

  // Reconnect Wi-Fi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi Disconnected. Reconnecting...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nWiFi Reconnected. IP: " + WiFi.localIP().toString());
    digitalWrite(ledPin, LOW);
    previousMillisLED = currentMillis;
  }

  // Read SCT-013 current
  double Irms = emon1.calcIrms(SCT013_NUM_SAMPLES);

  // Debug print
  Serial.print("Current (Irms): ");
  Serial.print(Irms, 3);
  Serial.println(" A");

  // Send to Flask server
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://" + String(flask_server_ip) + ":" + String(flask_port) + "/update";
    http.begin(client, url);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"field5\":" + String(Irms, 3) + "}";

    int httpCode = http.POST(payload);
    Serial.print("POST Status Code: ");
    Serial.println(httpCode);

    if (httpCode > 0) {
      String response = http.getString();
      Serial.println("Server Response: " + response);
    } else {
      Serial.printf("HTTPClient Error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  } else {
    Serial.println("WiFi not connected - data not sent.");
  }

  delay(5000);
}
