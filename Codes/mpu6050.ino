#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include <math.h>

// Wi-Fi credentials
const char* ssid = "Airtel_9962583910";
const char* password = "air10719";

// Laptop Flask server IP
const char* flask_server_ip = "192.168.1.3";  // Replace if different
const int flask_port = 5000;

// LED Pin and Blinking
const int ledPin = D4;
unsigned long previousMillisLED = 0;
const long blinkInterval = 500;
bool ledState = LOW;

// MPU6050 + WiFi Client
MPU6050 mpu;
WiFiClient client;

// Sensor raw values
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
  digitalWrite(ledPin, LOW);

  // I2C Setup and Scan
  Wire.begin();
  Serial.println("\n--- I2C Device Scan ---");
  byte error, address;
  int nDevices = 0;

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println(" !");
      nDevices++;
    } else if (error == 4) {
      Serial.print("Unknown error at 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  if (nDevices == 0) Serial.println("No I2C devices found");
  Serial.println("--- Scan Complete ---\n");

  // MPU6050 Setup
  Serial.println("Initializing MPU6050...");
  mpu.initialize();
  mpu.setSleepEnabled(false);
  delay(100);
  mpu.setClockSource(MPU6050_CLOCK_PLL_XGYRO);
  delay(100);
  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_1000);
  Serial.println("Gyroscope range set to ±1000 deg/s");
  Serial.println(mpu.testConnection() ? "MPU6050 OK (Setup Check)" : "MPU6050 FAIL (Setup Check)");

  Serial.println("Setup complete. Sending MPU6050 data to Flask server...");
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

  // Check MPU6050 connection in loop
  Serial.print("MPU6050 Connection Status (Loop Check): ");
  Serial.println(mpu.testConnection() ? "OK" : "FAIL");

  // Read sensor data
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  const float GYRO_SCALE = 32.8;
  float gyroX_dps = gx / GYRO_SCALE;
  float gyroY_dps = gy / GYRO_SCALE;
  float gyroZ_dps = gz / GYRO_SCALE;

  const float ACCEL_SCALE = 16384.0;
  float accelX_g = ax / ACCEL_SCALE;
  float accelY_g = ay / ACCEL_SCALE;
  float accelZ_g = az / ACCEL_SCALE;
  float euclidean_sum_accel = sqrt(accelX_g * accelX_g + accelY_g * accelY_g + accelZ_g * accelZ_g);

  // Debug print
  Serial.print("Gyro: X="); Serial.print(gyroX_dps);
  Serial.print(" Y="); Serial.print(gyroY_dps);
  Serial.print(" Z="); Serial.print(gyroZ_dps);
  Serial.print(" | Accel Sum: "); Serial.println(euclidean_sum_accel);

  // Send to Flask server
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://" + String(flask_server_ip) + ":" + String(flask_port) + "/update";
    http.begin(client, url);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"gyroX_dps\":" + String(gyroX_dps, 2) +
                     ",\"gyroY_dps\":" + String(gyroY_dps, 2) +
                     ",\"gyroZ_dps\":" + String(gyroZ_dps, 2) +
                     ",\"euclidean_sum_accel\":" + String(euclidean_sum_accel, 3) + "}";

    int httpCode = http.POST(payload);
    Serial.print("POST Status Code: ");
    Serial.println(httpCode);
    http.end();
  } else {
    Serial.println("WiFi not connected — data not sent.");
  }

  delay(5000);
}
