#include <ESP8266WiFi.h>        // Wi-Fi library
#include <ESP8266HTTPClient.h>  // HTTP requests library
#include "Wire.h"               // I2C communication library
#include "I2Cdev.h"             // MPU6050 dependency
#include "MPU6050.h"            // MPU6050 sensor library
#include <math.h>               // Math functions

// Wi-Fi credentials
const char* ssid = "ePlane-NBCC";
const char* password = "EkHanz-2025";

// ThingSpeak API details
const char* server = "http://api.thingspeak.com";
const char* apikey = "06F3WH65MFXRXPD8";

// LED Pin and Blinking
const int ledPin = D4;              // GPIO2 (D4)
unsigned long previousMillisLED = 0;
const long blinkInterval = 500;
bool ledState = LOW;

// MPU6050 object and WiFi client
MPU6050 mpu;
WiFiClient client;

// Raw sensor data
int16_t ax, ay, az;
int16_t gx, gy, gz;

void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH); // LED ON during Wi-Fi connect

  // Connect to Wi-Fi
  Serial.print("Connecting to WiFi: "); Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected. IP: " + WiFi.localIP().toString());
  digitalWrite(ledPin, LOW); // LED OFF after Wi-Fi connection

  // Initialize MPU6050
  Serial.println("Initializing MPU6050...");
  Wire.begin();
  mpu.initialize();

  // Set Gyroscope Full-Scale Range to ±250 deg/s (most sensitive)
  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_250);
  Serial.println("Gyroscope Full-Scale Range set to ±250 deg/s");

  // Check MPU6050 connection
  Serial.println(mpu.testConnection() ? "MPU6050 OK (Setup)" : "MPU6050 FAIL (Setup)");

  Serial.println("Setup complete. Sending MPU6050 data to ThingSpeak.");
}

void loop() {
  unsigned long currentMillis = millis();

  // Manage LED blinking
  if (WiFi.status() == WL_CONNECTED) {
    if (currentMillis - previousMillisLED >= blinkInterval) {
      previousMillisLED = currentMillis;
      ledState = !ledState;
      digitalWrite(ledPin, ledState ? HIGH : LOW);
    }
  } else {
    digitalWrite(ledPin, HIGH); // Always ON if Wi-Fi disconnected
  }

  // Reconnect Wi-Fi if disconnected
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

  // Read raw values from MPU6050
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Convert gyroscope raw values to degrees per second
  const float GYRO_SCALE = 131.0; // LSB sensitivity for ±250 deg/s
  float gyroX_dps = gx / GYRO_SCALE;
  float gyroY_dps = gy / GYRO_SCALE;
  float gyroZ_dps = gz / GYRO_SCALE;

  // Convert accelerometer raw values to g
  const float ACCEL_SCALE = 16384.0; // LSB/g for ±2g range
  float accelX_g = ax / ACCEL_SCALE;
  float accelY_g = ay / ACCEL_SCALE;
  float accelZ_g = az / ACCEL_SCALE;

  // Calculate magnitude of acceleration vector
  float euclidean_sum_accel = sqrt(accelX_g * accelX_g + accelY_g * accelY_g + accelZ_g * accelZ_g);

  // Debug prints
  Serial.print("Raw Gyro: X="); Serial.print(gx);
  Serial.print(" Y="); Serial.print(gy);
  Serial.print(" Z="); Serial.println(gz);

  Serial.print("AngVel(DPS): X="); Serial.print(gyroX_dps, 2);
  Serial.print(" Y="); Serial.print(gyroY_dps, 2);
  Serial.print(" Z="); Serial.print(gyroZ_dps, 2);
  Serial.print(" | Accel(g): "); Serial.println(euclidean_sum_accel, 3);

  // Send to ThingSpeak
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(server) + "/update?api_key=" + String(apikey) +
                 "&field1=" + String(gyroX_dps, 2) +
                 "&field2=" + String(gyroY_dps, 2) +
                 "&field3=" + String(gyroZ_dps, 2) +
                 "&field4=" + String(euclidean_sum_accel, 3);

    http.begin(client, url);
    int httpCode = http.GET();
    Serial.println("ThingSpeak HTTP Response Code: " + String(httpCode));
    http.end();
  } else {
    Serial.println("WiFi not connected for ThingSpeak update.");
  }

  delay(25000); // Delay for 25 seconds (free ThingSpeak rate limit is 15s)
}
