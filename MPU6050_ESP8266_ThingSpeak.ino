#include <ESP8266WiFi.h>      // Wi-Fi library
#include <ESP8266HTTPClient.h> // HTTP requests library
#include "Wire.h"             // I2C communication library
#include "I2Cdev.h"           // MPU6050 dependency
#include "MPU6050.h"          // MPU6050 sensor library
#include <math.h>             // Math functions (sqrt)

// Wi-Fi credentials
const char* ssid = "IC Startups";
const char* password = "IITM@IC_2047";

// ThingSpeak API details
const char* server = "http://api.thingspeak.com";
const char* apikey = "06F3WH65MFXRXPD8";

// LED Pin and Blinking
const int ledPin = D4;          // LED on GPIO2 (D4)
unsigned long previousMillisLED = 0; // Last LED update time
const long blinkInterval = 500; // LED blink interval in ms
bool ledState = LOW;            // Current LED state

WiFiClient client; // WiFiClient object
MPU6050 mpu;       // MPU6050 sensor object

// MPU6050 raw data variables
int16_t ax, ay, az; // Raw Accelerometer values
int16_t gx, gy, gz; // Raw Gyroscope values

void setup() {
  Serial.begin(115200); // Start serial communication
  delay(100);           // Allow serial to begin

  pinMode(ledPin, OUTPUT); // Set LED pin as output
  digitalWrite(ledPin, HIGH); // LED ON during WiFi connect

  // Connect to Wi-Fi
  Serial.print("Connecting to WiFi: "); Serial.println(ssid);
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nWiFi Connected. IP: " + WiFi.localIP().toString());
  digitalWrite(ledPin, LOW); // LED OFF after WiFi connected

  // Initialize MPU6050
  Serial.println("Initializing MPU6050...");
  Wire.begin();       // Start I2C bus
  mpu.initialize();   // Initialize MPU6050 sensor

  // --- FIX: Explicitly set Gyro Full-Scale Range ---
  // This sets the gyroscope to +/- 250 degrees/second (most sensitive)
  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_250);
  Serial.println("Gyroscope Full-Scale Range set to +/- 250 deg/s.");

  // Test connection here at setup for initial check
  Serial.println(mpu.testConnection() ? "MPU6050 OK (Setup)" : "MPU6050 FAIL (Setup)");
  delay(100); // Brief delay after MPU6050 init

  Serial.println("Setup Complete. Sending MPU6050 data to ThingSpeak.");
}

void loop() {
  unsigned long currentMillis = millis(); // Get current time

  // Manage LED blinking
  if (WiFi.status() == WL_CONNECTED) {
    if (currentMillis - previousMillisLED >= blinkInterval) {
      previousMillisLED = currentMillis; // Update timer
      ledState = !ledState;              // Toggle LED state
      digitalWrite(ledPin, ledState ? HIGH : LOW); // Update LED
    }
  } else {
    digitalWrite(ledPin, HIGH); // LED ON if WiFi disconnected
  }

  // Reconnect Wi-Fi if needed
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi Disconnected. Reconnecting...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
    Serial.println("\nWiFi Reconnected. IP: " + WiFi.localIP().toString());
    digitalWrite(ledPin, LOW); // LED OFF after reconnect
    previousMillisLED = currentMillis; // Reset LED timer
  }

  // --- MPU6050 Connection Test in Loop ---
  Serial.println("MPU6050 Connection: " + String(mpu.testConnection() ? "OK" : "FAIL")); // Test connection in loop

  // Read MPU6050 data
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // --- Print Raw Gyroscope Values (for debugging) ---
  Serial.print("Raw Gyro: X="); Serial.print(gx);
  Serial.print(" Y="); Serial.print(gy);
  Serial.print(" Z="); Serial.print(gz);
  Serial.println();

  // Calculate Angular Velocity (DPS)
  float gyroX_dps = mpu.getRotationX();
  float gyroY_dps = mpu.getRotationY();
  float gyroZ_dps = mpu.getRotationZ();

  // Calculate Euclidean Sum of Acceleration (g)
  const float ACCEL_SCALE_FACTOR = 16384.0; // For +/- 2g range
  float accelX_g = (float)ax / ACCEL_SCALE_FACTOR;
  float accelY_g = (float)ay / ACCEL_SCALE_FACTOR;
  float accelZ_g = (float)az / ACCEL_SCALE_FACTOR;
  float euclidean_sum_accel = sqrt(accelX_g*accelX_g + accelY_g*accelY_g + accelZ_g*accelZ_g);

  // Print data to Serial Monitor
  Serial.print("AngVel(DPS): X="); Serial.print(gyroX_dps, 2);
  Serial.print(" Y="); Serial.print(gyroY_dps, 2);
  Serial.print(" Z="); Serial.print(gyroZ_dps, 2);
  Serial.print(" Accel(g): "); Serial.println(euclidean_sum_accel, 3);

  // Send data to ThingSpeak
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(server) + "/update?api_key=" + String(apikey) +
                 "&field1=" + String(gyroX_dps, 2) +
                 "&field2=" + String(gyroY_dps, 2) +
                 "&field3=" + String(gyroZ_dps, 2) +
                 "&field4=" + String(euclidean_sum_accel, 3);

    http.begin(client, url); // Begin HTTP request
    int httpCode = http.GET(); // Send GET request
    Serial.println("TS HTTP Code: " + String(httpCode)); // Print response code
    http.end(); // End HTTP session
  } else {
    Serial.println("WiFi not connected for TS update.");
  }

  delay(25000); // Wait before next update (Remember ThingSpeak's free tier has a 15-second update limit!)
}