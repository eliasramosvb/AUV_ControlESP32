#include "Pins.h"
#include "Motors.h"
#include "Battery.h"
#include "SerialProtocol.h"

// =====================================================
// Global state
// =====================================================

static unsigned long gHeartbeatIntervalMs = 1000;
static unsigned long gLastHeartbeatTimeMs = 0;
static bool gHeartbeatEnabled = false;

static int gCurrentM1 = 0;
static int gCurrentM2 = 0;
static int gCurrentM3 = 0;

static unsigned long gLastBatteryTelemetryMs = 0;
static const unsigned long BATTERY_TELEMETRY_INTERVAL_MS = 1000;

// =====================================================
// Local helpers
// =====================================================

void applyMotorOutputs(int m1, int m2, int m3) {
  // These functions are implemented in Motors.cpp
  setMotorSpeed(1, m1);
  setMotorSpeed(2, m2);
  setMotorSpeed(3, m3);

  gCurrentM1 = m1;
  gCurrentM2 = m2;
  gCurrentM3 = m3;
}

void stopAllOutputs() {
  // Implemented in Motors.cpp
  stopAllMotors();

  gCurrentM1 = 0;
  gCurrentM2 = 0;
  gCurrentM3 = 0;
}

void sendBatteryTelemetry() {
  // Implemented in Battery.cpp
  float vBat = readBatteryVoltage();

  // Implemented in SerialProtocol.cpp
  Serial.print(formatBatteryMessage(vBat, gCurrentM1, gCurrentM2, gCurrentM3));
}

void sendMotorTelemetry() {
  // Implemented in SerialProtocol.cpp
  Serial.print(formatMotorStatusMessage(gCurrentM1, gCurrentM2, gCurrentM3));
}

void sendErrorTelemetry(int errorCode) {
  // Implemented in SerialProtocol.cpp
  Serial.print(formatErrorMessage(errorCode));
}

void handleDirectControl(const SerialCommand &cmd) {
  applyMotorOutputs(cmd.m1, cmd.m2, cmd.m3);
  sendMotorTelemetry();
}

void handleHighLevelControl(const SerialCommand &cmd) {
  // Protocol mapping:
  // M1 = forward + turn
  // M2 = forward - turn
  // M3 = vertical

  int m1 = cmd.forward + cmd.turn;
  int m2 = cmd.forward - cmd.turn;
  int m3 = cmd.vertical;

  applyMotorOutputs(m1, m2, m3);
  sendMotorTelemetry();
}

void handleHeartbeat(const SerialCommand &cmd) {
  gHeartbeatIntervalMs = cmd.heartbeatIntervalMs;
  gLastHeartbeatTimeMs = millis();
  gHeartbeatEnabled = true;
}

void checkHeartbeatTimeout() {
  if (!gHeartbeatEnabled) {
    return;
  }

  unsigned long now = millis();
  unsigned long timeoutMs = gHeartbeatIntervalMs * 2;

  if (timeoutMs < 100) {
    timeoutMs = 100;
  }

  if (now - gLastHeartbeatTimeMs > timeoutMs) {
    stopAllOutputs();
    sendErrorTelemetry(1);
    gHeartbeatEnabled = false;
  }
}

void sendPeriodicTelemetry() {
  unsigned long now = millis();

  if (now - gLastBatteryTelemetryMs >= BATTERY_TELEMETRY_INTERVAL_MS) {
    gLastBatteryTelemetryMs = now;
    sendBatteryTelemetry();
  }
}

// =====================================================
// Arduino setup / loop
// =====================================================

void setup() {
  Serial.begin(115200);
  delay(2000);

  // Implemented in Motors.cpp
  initMotors();

  // Implemented in Battery.cpp
  initBattery();

  // Implemented in SerialProtocol.cpp
  initSerialProtocol();

  stopAllOutputs();

  Serial.println("ESP32 ready");
  Serial.print(formatFeaturesMessage());
  sendBatteryTelemetry();
  sendMotorTelemetry();
}

void loop() {
  SerialCommand cmd;

  // Implemented in SerialProtocol.cpp
if (readSerialCommand(cmd)) {
  switch (cmd.type) {
    case CMD_DIRECT_CONTROL:
      handleDirectControl(cmd);
      break;

    case CMD_HIGH_LEVEL_CONTROL:
      handleHighLevelControl(cmd);
      break;

    case CMD_HEARTBEAT:
      handleHeartbeat(cmd);
      break;

    case CMD_FEATURE_REQUEST:
      Serial.print(formatFeaturesMessage());
      break;

    case CMD_BATTERY_REQUEST:
      sendBatteryTelemetry();
      break;

    case CMD_UNKNOWN:
      sendErrorTelemetry(1);
      break;

    default:
      break;
  }
}

  checkHeartbeatTimeout();
  sendPeriodicTelemetry();
}