
// config-pins.h
//
// Arduino output pin configuration for all bot modules.

#pragma once


// LED matrix DI pin
#define LED_PIN           GPIO_NUM_40

// Servo motor control pins
// x axis servo - SERVO1
#define SERVO1_PIN        GPIO_NUM_5
// y axis servo - SERVO2
#define SERVO2_PIN        GPIO_NUM_4
// other unused servo ports - GPIO_NUM_6 and GPIO_NUM_8

// DC motor pins
#define MOTOR_LEFT_PIN1   GPIO_NUM_14
#define MOTOR_LEFT_PIN2   GPIO_NUM_13
#define MOTOR_RIGHT_PIN1  GPIO_NUM_12
#define MOTOR_RIGHT_PIN2  GPIO_NUM_11

#define MOTOR_ENABLE_PIN  GPIO_NUM_38
