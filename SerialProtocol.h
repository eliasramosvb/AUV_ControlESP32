#ifndef SERIALPROTOCOL_H
#define SERIALPROTOCOL_H

#include <Arduino.h>

// =====================================================
// Command types from app to ESP32
// =====================================================
enum CommandType {
  CMD_NONE,
  CMD_DIRECT_CONTROL,     // c<M1>,<M2>,<M3>
  CMD_HIGH_LEVEL_CONTROL, // u<forward>,<turn>,<vertical>
  CMD_HEARTBEAT,          // h<interval_ms>
  CMD_FEATURE_REQUEST,    // f
  CMD_BATTERY_REQUEST,    // bat
  CMD_UNKNOWN
};

// =====================================================
// Parsed command container
// =====================================================
struct SerialCommand {
  CommandType type = CMD_NONE;
  String raw = "";

  // For direct motor control: c<M1>,<M2>,<M3>
  int m1 = 0;
  int m2 = 0;
  int m3 = 0;

  // For high-level control: u<forward>,<turn>,<vertical>
  int forward = 0;
  int turn = 0;
  int vertical = 0;

  // For heartbeat: h<interval_ms>
  unsigned long heartbeatIntervalMs = 0;
};

// =====================================================
// Initialization
// =====================================================
void initSerialProtocol();

// =====================================================
// Read and parse one full serial command line
// Returns true if a complete command was received
// =====================================================
bool readSerialCommand(SerialCommand &cmd);

// =====================================================
// Helpers
// =====================================================
const char* commandTypeToString(CommandType type);

// =====================================================
// Telemetry format helpers (ESP32 -> App)
// =====================================================
String formatDepthMessage(float depthMeters);                         // d<depth>\n
String formatBatteryMessage(float voltage, int m1, int m2, int m3);  // bat,<voltage>,<m1>,<m2>,<m3>\n
String formatMotorStatusMessage(int m1, int m2, int m3);             // m<M1>,<M2>,<M3>\n
String formatErrorMessage(int errorCode);                            // e<code>\n
String formatFeaturesMessage();                                      // response to f

#endif