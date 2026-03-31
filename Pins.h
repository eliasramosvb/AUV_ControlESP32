#ifndef PINS_H
#define PINS_H

// Inverte direção dos motores (1 = normal, -1 = invertido)
#define MOTOR1_DIRECTION -1
#define MOTOR2_DIRECTION -1
#define MOTOR3_DIRECTION 1


// =====================================================
// Projeto AUV - ESP32-S3-N16R8
// Definicao centralizada de pinos
// =====================================================

// =========================
// Motores - 3x MPQ6612A
// =========================
static const int M1_IN1_PIN = 5;
static const int M1_IN2_PIN = 4;

static const int M2_IN1_PIN = 6;
static const int M2_IN2_PIN = 7;

static const int M3_IN1_PIN = 15;
static const int M3_IN2_PIN = 16;

// =========================
// PWM channels for motors
// These are software channels, not GPIO numbers
// =========================
static const int M1_IN1_PWM_CH = 0;
static const int M1_IN2_PWM_CH = 1;

static const int M2_IN1_PWM_CH = 2;
static const int M2_IN2_PWM_CH = 3;

static const int M3_IN1_PWM_CH = 4;
static const int M3_IN2_PWM_CH = 5;

static const int MOTOR_PWM_FREQ = 20000;    // 20 kHz
static const int MOTOR_PWM_RES  = 8;        // 8-bit: 0 to 255
static const int MOTOR_PWM_MAX  = 255;


// Observacao:
// O pino SLEEP dos drivers MPQ6612A esta ligado direto em 3V3,
// entao nao ha pino de controle via software.

// =========================
// MicroSD - SPI
// =========================
static const int SD_CS_PIN   = 12;
static const int SD_MOSI_PIN = 8;
static const int SD_MISO_PIN = 9;
static const int SD_SCK_PIN  = 10;

// =========================
// I2C compartilhado
// MPU9250 + HW-611 + LPS3x
// =========================
static const int I2C_SCL_PIN = 13;
static const int I2C_SDA_PIN = 14;

// =========================
// Endereco I2C do LPS3x
// =========================
static const uint8_t LPS3X_I2C_ADDRESS = 0x5C;

// =========================
// Monitoramento de bateria
// =========================
static const int VBAT_SENSE_PIN = 1;   // GPIO1 = ADC1_CH0

// Divisor resistivo
static const float VBAT_R1 = 200000.0f; // ohms
static const float VBAT_R2 = 100000.0f; // ohms

// =========================
// Restricoes importantes
// =========================
// Nao usar:
// GPIO43 / GPIO44 -> UART0
// GPIO19 / GPIO20 -> USB nativo

#endif