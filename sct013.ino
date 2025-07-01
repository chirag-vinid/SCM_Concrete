#include "EmonLib.h"

EnergyMonitor emon1;

const int ANALOG_PIN = A0;
const float CALIBRATION_CONSTANT = 30.0;
const int NUM_SAMPLES = 1480;

void setup() {
  Serial.begin(115200);

  emon1.current(ANALOG_PIN, CALIBRATION_CONSTANT);

  delay(1000);
}

void loop() {
  double Irms = emon1.calcIrms(NUM_SAMPLES);

  Serial.print("Current: ");
  Serial.print(Irms, 3);
  Serial.println(" A");

  if (Irms > 0.5) {
    digitalWrite(D4, HIGH);
  } else {
    digitalWrite(D4, LOW);
  }

  delay(2000);
}
