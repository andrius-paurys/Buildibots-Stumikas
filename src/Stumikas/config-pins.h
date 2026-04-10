
// config-pins.h
//
// Arduino output pin configuration for all bot modules.

#pragma once


// LED matrix DI pin
#define LED_PIN           GPIO_NUM_15

// Servo motor control pins
#define SERVO1_PIN        GPIO_NUM_1
#define SERVO2_PIN        GPIO_NUM_14

// DC motor pins
#define MOTOR_LEFT_PIN1   GPIO_NUM_14
#define MOTOR_LEFT_PIN2   GPIO_NUM_13
#define MOTOR_RIGHT_PIN1  GPIO_NUM_12
#define MOTOR_RIGHT_PIN2  GPIO_NUM_11

#define MOTOR_ENABLE_PIN  GPIO_NUM_38
