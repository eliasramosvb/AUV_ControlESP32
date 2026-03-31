#include "Battery.h"
#include "Pins.h"

static const float ADC_REFERENCE_VOLTAGE = 3.3f;
static const int ADC_MAX_VALUE = 4095;   // 12-bit ADC

void initBattery() {
  pinMode(VBAT_SENSE_PIN, INPUT);

  // ESP32 Arduino ADC configuration
  analogReadResolution(12);
}

int readBatteryRaw() {
  return analogRead(VBAT_SENSE_PIN);
}

float readBatterySenseVoltage() {
  int raw = readBatteryRaw();
  return (raw * ADC_REFERENCE_VOLTAGE) / ADC_MAX_VALUE;
}

float readBatteryVoltage() {
  float vSense = readBatterySenseVoltage();

  // Voltage divider formula:
  // Vbat = Vsense * (R1 + R2) / R2
  float vBat = vSense * (VBAT_R1 + VBAT_R2) / VBAT_R2;

  return vBat;
}