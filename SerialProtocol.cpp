#include "SerialProtocol.h"

// =====================================================
// Private helpers
// =====================================================

static String readLineFromSerial() {
  if (!Serial.available()) {
    return "";
  }

  String line = Serial.readStringUntil('\n');
  line.trim();
  return line;
}

//static int clampSigned255(int value) {
//  if (value > 255) return 255;
//  if (value < -255) return -255;
//  return value;
//}

static bool parseTwoInts(const String &body, int &a, int &b) {
  int firstComma = body.indexOf(',');
  if (firstComma < 0) return false;

  String aStr = body.substring(0, firstComma);
  String bStr = body.substring(firstComma + 1);

  aStr.trim();
  bStr.trim();

  if (aStr.length() == 0 || bStr.length() == 0) {
    return false;
  }

  a = aStr.toInt();
  b = bStr.toInt();
  return true;
}

static bool parseThreeInts(const String &body, int &a, int &b, int &c) {
  int firstComma = body.indexOf(',');
  if (firstComma < 0) return false;

  int secondComma = body.indexOf(',', firstComma + 1);
  if (secondComma < 0) return false;

  String aStr = body.substring(0, firstComma);
  String bStr = body.substring(firstComma + 1, secondComma);
  String cStr = body.substring(secondComma + 1);

  aStr.trim();
  bStr.trim();
  cStr.trim();

  if (aStr.length() == 0 || bStr.length() == 0 || cStr.length() == 0) {
    return false;
  }

  a = aStr.toInt();
  b = bStr.toInt();
  c = cStr.toInt();
  return true;
}

static bool parseUnsignedLong(const String &body, unsigned long &value) {
  if (body.length() == 0) {
    return false;
  }

  for (unsigned int i = 0; i < body.length(); i++) {
    if (!isDigit(body[i])) {
      return false;
    }
  }

  value = body.toInt();
  return true;
}

// Accept both:
// c100,100,0
// c,100,100,0
// c100,100
// c,100,100
static bool parseDirectControl(const String &line, SerialCommand &cmd) {
  String body = line.substring(1);
  body.trim();

  if (body.startsWith(",")) {
    body = body.substring(1);
    body.trim();
  }

  int a = 0, b = 0, c = 0;

  if (parseThreeInts(body, a, b, c)) {
    cmd.type = CMD_DIRECT_CONTROL;
//    cmd.m1 = clampSigned255(a);
//    cmd.m2 = clampSigned255(b);
//    cmd.m3 = clampSigned255(c);
    cmd.m1 = a;
    cmd.m2 = b;
    cmd.m3 = c;
    return true;
  }

  if (parseTwoInts(body, a, b)) {
    cmd.type = CMD_DIRECT_CONTROL;
//    cmd.m1 = clampSigned255(a);
//    cmd.m2 = clampSigned255(b);
    cmd.m1 = a;
    cmd.m2 = b;
    cmd.m3 = 0;
    return true;
  }

  return false;
}

// Accept both:
// u80,20,50
// u,80,20,50
// u80,20
// u,80,20
static bool parseHighLevelControl(const String &line, SerialCommand &cmd) {
  String body = line.substring(1);
  body.trim();

  if (body.startsWith(",")) {
    body = body.substring(1);
    body.trim();
  }

  int a = 0, b = 0, c = 0;

  if (parseThreeInts(body, a, b, c)) {
    cmd.type = CMD_HIGH_LEVEL_CONTROL;
    //cmd.forward = clampSigned255(a);
    //cmd.turn = clampSigned255(b);
    //cmd.vertical = clampSigned255(c);
    cmd.forward = a;
    cmd.turn = b;
    cmd.vertical = c;
    return true;
  }

  if (parseTwoInts(body, a, b)) {
    cmd.type = CMD_HIGH_LEVEL_CONTROL;
    //cmd.forward = clampSigned255(a);
    //cmd.turn = clampSigned255(b);
    cmd.forward = a;
    cmd.turn = b;
    cmd.vertical = 0;
    return true;
  }

  return false;
}

// =====================================================
// Public API
// =====================================================

void initSerialProtocol() {
}

bool readSerialCommand(SerialCommand &cmd) {
  cmd = SerialCommand();

  String line = readLineFromSerial();
  if (line.length() == 0) {
    return false;
  }

  cmd.raw = line;
  line.trim();

  // comando textual: bat
  if (line.equalsIgnoreCase("bat")) {
    cmd.type = CMD_BATTERY_REQUEST;
    return true;
  }

  char header = line.charAt(0);

  switch (header) {
    case 'c':
      if (parseDirectControl(line, cmd)) {
        return true;
      }
      cmd.type = CMD_UNKNOWN;
      return true;

    case 'u':
      if (parseHighLevelControl(line, cmd)) {
        return true;
      }
      cmd.type = CMD_UNKNOWN;
      return true;

    case 'h': {
      String body = line.substring(1);
      body.trim();

      if (body.startsWith(",")) {
        body = body.substring(1);
        body.trim();
      }

      unsigned long intervalMs = 0;

      if (!parseUnsignedLong(body, intervalMs)) {
        cmd.type = CMD_UNKNOWN;
        return true;
      }

      cmd.type = CMD_HEARTBEAT;
      cmd.heartbeatIntervalMs = intervalMs;
      return true;
    }

    case 'f':
      if (line == "f") {
        cmd.type = CMD_FEATURE_REQUEST;
        return true;
      }
      cmd.type = CMD_UNKNOWN;
      return true;

    default:
      cmd.type = CMD_UNKNOWN;
      return true;
  }
}

const char* commandTypeToString(CommandType type) {
  switch (type) {
    case CMD_NONE:
      return "CMD_NONE";
    case CMD_DIRECT_CONTROL:
      return "CMD_DIRECT_CONTROL";
    case CMD_HIGH_LEVEL_CONTROL:
      return "CMD_HIGH_LEVEL_CONTROL";
    case CMD_HEARTBEAT:
      return "CMD_HEARTBEAT";
    case CMD_FEATURE_REQUEST:
      return "CMD_FEATURE_REQUEST";
    case CMD_BATTERY_REQUEST:
      return "CMD_BATTERY_REQUEST";
    case CMD_UNKNOWN:
      return "CMD_UNKNOWN";
    default:
      return "INVALID_COMMAND_TYPE";
  }
}

String formatDepthMessage(float depthMeters) {
  String msg = "d";
  msg += String(depthMeters, 2);
  msg += "\n";
  return msg;
}

// novo formato:
// bat,<voltage>,<m1>,<m2>,<m3>
String formatBatteryMessage(float voltage, int m1, int m2, int m3) {
  String msg = "bat,";
  msg += String(voltage, 2);
  msg += ",";
  msg += String(m1);
  msg += ",";
  msg += String(m2);
  msg += ",";
  msg += String(m3);
  msg += "\n";
  return msg;
}

String formatMotorStatusMessage(int m1, int m2, int m3) {
  String msg = "m";
  //msg += String(clampSigned255(m1));
  msg += String(m1);
  msg += ",";
  msg += String(m2);
  msg += ",";
  msg += String(m3);
  msg += "\n";
  return msg;
}

String formatErrorMessage(int errorCode) {
  String msg = "e";
  msg += String(errorCode);
  msg += "\n";
  return msg;
}

String formatFeaturesMessage() {
  return "fmotors=3,battery=1,depth=0,heartbeat=1\n";
}