#include <ESP8266WiFi.h>        // Wi-Fi library
#include <ESP8266HTTPClient.h>  // HTTP requests library
#include "Wire.h"               // I2C communication library
#include "I2Cdev.h"             // MPU6050 dependency
#include "MPU6050.h"            // MPU6050 sensor library
#include <math.h>               // Math functions

// Wi-Fi credentials
const char* ssid = "IC Startups";
const char* password = "IITM@IC_2047";

// ThingSpeak API details
const char* server = "http://api.thingspeak.com";
const char* apikey = "06F3WH65MFXRXPD8";

// LED Pin and Blinking
const int ledPin = D4;          // GPIO2 (D4) - Note: D4 on NodeMCU is GPIO2
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

  // Initialize I2C communication
  Wire.begin();

  // --- Start I2C Scanner Test ---
  Serial.println("\n--- I2C Device Scan ---");
  byte error, address;
  int nDevices;

  nDevices = 0;
  for (address = 1; address < 127; address++) {
    // We use the requestFrom() method to check if a device acknowledges
    // the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) {
        Serial.print("0"); // Pad with leading zero for consistent formatting
      }
      Serial.print(address, HEX);
      Serial.println("   !");
      nDevices++;
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  } else {
    Serial.println("--- Scan Complete ---\n");
  }
  // --- End I2C Scanner Test ---

  // Initialize MPU6050
  Serial.println("Initializing MPU6050...");
  mpu.initialize(); // Initialize MPU6050

  // --- Explicitly ensure MPU6050 is awake and stable ---
  // This helps in cases where initialize() alone isn't sufficient after power-up.
  mpu.setSleepEnabled(false); // Ensure device is awake
  delay(100); // Small delay after waking up for stability
  mpu.setClockSource(MPU6050_CLOCK_PLL_XGYRO); // Set a stable clock source (PLL with X Gyro reference)
  delay(100); // Small delay
  // --- End explicit initialization commands ---

  // Set Gyroscope Full-Scale Range to ±1000 deg/s (MPU6050_GYRO_FS_1000 for 1000 deg/s)
  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_1000);
  Serial.println("Gyroscope Full-Scale Range set to ±1000 deg/s");

  // Check MPU6050 connection during setup
  Serial.println(mpu.testConnection() ? "MPU6050 OK (Setup Check)" : "MPU6050 FAIL (Setup Check)");

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
    previousMillisLED = currentMillis; // Reset LED timer after reconnect
  }

  // Check MPU6050 connection periodically
  Serial.print("MPU6050 Connection Status (Loop Check): ");
  Serial.println(mpu.testConnection() ? "OK" : "FAIL");

  // Read raw values from MPU6050
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Convert gyroscope raw values to degrees per second
  // MPU6050_GYRO_FS_1000 has a sensitivity of 32.8 LSB/°/s
  const float GYRO_SCALE = 32.8;
  float gyroX_dps = gx / GYRO_SCALE;
  float gyroY_dps = gy / GYRO_SCALE;
  float gyroZ_dps = gz / GYRO_SCALE;

  // Convert accelerometer raw values to g
  // MPU6050 default accel range is ±2g, sensitivity is 16384 LSB/g
  const float ACCEL_SCALE = 16384.0;
  float accelX_g = ax / ACCEL_SCALE;
  float accelY_g = ay / ACCEL_SCALE;
  float accelZ_g = az / ACCEL_SCALE;

  // Calculate magnitude of acceleration vector
  float euclidean_sum_accel = sqrt(accelX_g * accelX_g + accelY_g * accelY_g + accelZ_g * accelZ_g);

  // Debug prints for sensor data
  Serial.print("Raw Gyro: X="); Serial.print(gx);
  Serial.print(" Y="); Serial.print(gy);
  Serial.print(" Z="); Serial.println(gz);

  Serial.print("AngVel(DPS): X="); Serial.print(gyroX_dps, 2);
  Serial.print(" Y="); Serial.print(gyroY_dps, 2);
  Serial.print(" Z="); Serial.print(gyroZ_dps, 2);
  Serial.print(" | Accel(g) Euclidean Sum: "); Serial.println(euclidean_sum_accel, 3);


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

  // Delay for 20 seconds (including the time taken for reading and sending data)
  // Adjusted from 25s to 20s as requested implicitly by "every 20 seconds".
  delay(20000);
}
