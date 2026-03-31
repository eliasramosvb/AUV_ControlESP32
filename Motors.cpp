#include "Motors.h"
#include "Pins.h"

// =====================================================
// Private helper functions
// =====================================================
//
static int clampSpeed(int speed) {
  if (speed > MOTOR_PWM_MAX) return MOTOR_PWM_MAX;
  if (speed < -MOTOR_PWM_MAX) return -MOTOR_PWM_MAX;
  return speed;
}

static void stopMotorPins(int pinA, int pinB) {
  ledcWrite(pinA, 0);
  ledcWrite(pinB, 0);
}

static void setMotorPins(int pinForward, int pinReverse, int speed) {
  speed = clampSpeed(speed);

  if (speed > 0) {
    ledcWrite(pinForward, speed);
    ledcWrite(pinReverse, 0);
  } 
  else if (speed < 0) {
    ledcWrite(pinForward, 0);
    ledcWrite(pinReverse, -speed);
  } 
  else {
    ledcWrite(pinForward, 0);
    ledcWrite(pinReverse, 0);
  }
}

// =====================================================
// Public functions
// =====================================================

void initMotors() {
  pinMode(M1_IN1_PIN, OUTPUT);
  pinMode(M1_IN2_PIN, OUTPUT);

  pinMode(M2_IN1_PIN, OUTPUT);
  pinMode(M2_IN2_PIN, OUTPUT);

  pinMode(M3_IN1_PIN, OUTPUT);
  pinMode(M3_IN2_PIN, OUTPUT);

  // Attach PWM directly to pins
  ledcAttach(M1_IN1_PIN, MOTOR_PWM_FREQ, MOTOR_PWM_RES);
  ledcAttach(M1_IN2_PIN, MOTOR_PWM_FREQ, MOTOR_PWM_RES);

  ledcAttach(M2_IN1_PIN, MOTOR_PWM_FREQ, MOTOR_PWM_RES);
  ledcAttach(M2_IN2_PIN, MOTOR_PWM_FREQ, MOTOR_PWM_RES);

  ledcAttach(M3_IN1_PIN, MOTOR_PWM_FREQ, MOTOR_PWM_RES);
  ledcAttach(M3_IN2_PIN, MOTOR_PWM_FREQ, MOTOR_PWM_RES);

  stopAllMotors();
}

void stopAllMotors() {
  stopMotorPins(M1_IN1_PIN, M1_IN2_PIN);
  stopMotorPins(M2_IN1_PIN, M2_IN2_PIN);
  stopMotorPins(M3_IN1_PIN, M3_IN2_PIN);
}

void setMotorSpeed(uint8_t motorIndex, int speed) {
  switch (motorIndex) {
    case 1:
      speed *= MOTOR1_DIRECTION;
      setMotorPins(M1_IN1_PIN, M1_IN2_PIN, speed);
      break;

    case 2:
      speed *= MOTOR2_DIRECTION;
      setMotorPins(M2_IN1_PIN, M2_IN2_PIN, speed);
      break;

    case 3:
      speed *= MOTOR3_DIRECTION;
      setMotorPins(M3_IN1_PIN, M3_IN2_PIN, speed);
      break;

    default:
      break;
  }
}