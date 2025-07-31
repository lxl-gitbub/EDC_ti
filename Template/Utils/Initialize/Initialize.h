#ifndef INITIALIZE_H
#define INITIALIZE_H

// �˶�ѧͷ�ļ�������������ֱ��ת�亯��
#include "kinematics.h"
#include "AllHeader.h"

// Parameters for the encoder
#define PPR 13  // The Pulses Per Revolution of encoder

// Parameters for the motor
#define REDUCE 28
#define FULL_SPEED_RPM 300

// Parameters for the wheel
#define WHEEL_DIAMETER 65 // The diameter of the wheel in mm
#define WHEEL_DIS      115

// ====================电机和编码器参数定义====================
// 根据重构的驱动修改初始化参数

// 右电机初始化参数 (对应Motor_UI_Init(&Right, ...))
#define RIGHT_MOTOR_IN1_PORT    Motor_IO_AIN1_PORT
#define RIGHT_MOTOR_IN1_PIN     Motor_IO_AIN1_PIN
#define RIGHT_MOTOR_IN2_PORT    Motor_IO_AIN2_PORT
#define RIGHT_MOTOR_IN2_PIN     Motor_IO_AIN2_PIN
#define RIGHT_MOTOR_PWM_TIMER   Motor_INST
#define RIGHT_MOTOR_PWM_CHANNEL DL_TIMER_CC_0_INDEX
#define RIGHT_MOTOR_INIT_DUTY   0

// 左电机初始化参数 (对应Motor_UI_Init(&Left, ...))
#define LEFT_MOTOR_IN1_PORT     Motor_IO_BIN1_PORT
#define LEFT_MOTOR_IN1_PIN      Motor_IO_BIN1_PIN
#define LEFT_MOTOR_IN2_PORT     Motor_IO_BIN2_PORT
#define LEFT_MOTOR_IN2_PIN      Motor_IO_BIN2_PIN
#define LEFT_MOTOR_PWM_TIMER    Motor_INST
#define LEFT_MOTOR_PWM_CHANNEL  DL_TIMER_CC_1_INDEX
#define LEFT_MOTOR_INIT_DUTY    0

// 编码器初始化参数 (单个编码器，不使用LR模式)
#define ENCODER_QEI_TIMER       QEI_0_INST
#define ENCODER_REAL_TIMER      (GPTIMER_Regs*)TIMA0_BASE


#ifdef __cplusplus
extern "C" {
#endif
extern CarState car; // Declare car state for kinematics
extern Data current_data; // Declare current data for kinematics
extern uint8_t Digital[8]; // Declare an array for digital sensor readings
extern char error_message[100]; // Buffer for error messages
extern double GyroscopeChannelData[10]; // Array to hold gyroscope data
extern uint32_t tick; // Global tick counter for timing

void MECInit();
void LMotorSet(MOVETYPE type, uint16_t duty);
void RMotorSet(MOVETYPE type, uint16_t duty);

void LSet(int16_t duty);
void RSet(int16_t duty);

void Break(); // Set both motors to break mode
// This function sets both motors to break mode, stopping them quickly

// RotationAngles getRotationAngles(); // Get the current rotation angles from the MS601M sensor
float getYaw(); // Get the current yaw angle from the MS601M sensor
float getWz(); // Get the current angular velocity (wz) from the MS601M sensor
float CalibrateYawOffset(); // Calibrate the yaw offset

// Get the current data and update the car state
void UpdateData(); // Update the current data with the latest sensor readings
void UpdateData_Car(); // Update the car state with the latest sensor readings

void error_handler(void); // Handle errors by stopping motors and displaying error messages
void delay_cycles(uint32_t cycles); // Simple delay function for TI platform

#ifdef __cplusplus
}
#endif
#endif /* INITIALIZE_H */