#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>

enum MotorIndex {
  MOTOR_1 = 1,
  MOTOR_2 = 2,
  MOTOR_3 = 3
};

void initMotors();
void stopAllMotors();

// Função principal de controle
void setMotorSpeed(uint8_t motorIndex, int speed);

#endif