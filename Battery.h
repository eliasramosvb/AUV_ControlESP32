#ifndef BATTERY_H
#define BATTERY_H

#include <Arduino.h>

// Initialize battery sensing pin
void initBattery();

// Read raw ADC value
int readBatteryRaw();

// Read battery voltage at the ADC pin
float readBatterySenseVoltage();

// Read actual battery voltage (before divider)
float readBatteryVoltage();

#endif