
// Stumikas-Bucket.cpp
//
// Provides functions for controlling the bot's loading bucket servos.

#include "Stumikas-Bucket.h"

#include <Arduino.h>
#include <ESP32Servo.h>
#include "config-pins.h"


// Measured angle clearances of the bot body
#define SERVO1_MIN -25
#define SERVO1_MAX 40
#define SERVO2_MIN -80
#define SERVO2_MAX 40

// PWM signal characteristic
#define MIN_PULSE_WIDTH 500
#define MAX_PULSE_WIDTH 2500


// ESP32Servo objects for servo control
static Servo servo1, servo2;


/**
 * Setup PWM output for loading bucket control.
 * @return void
 */
void bucket_setup() {
  servo1.attach(SERVO1_PIN, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  servo2.attach(SERVO2_PIN, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
}


/**
 * @brief Adjusts servo angles to control the bot's loading bucket.
 *
 * Accepts angle values from -90 to 90, relative to the bucket's centered position.
 * Values will be trimmed to respect real measured physical bucket clearances.
 *
 * @param angleX Desired horizontal angle for the bucket (-90 to 90, relative to center).
 * @param angleY Desired vertical angle for the bucket (-90 to 90, relative to center).
 */
void bucket_tick(int angleX, int angleY) {

  // Hard-limit angles of the servos
  angleX = constrain(angleX, SERVO1_MIN, SERVO1_MAX);
  angleY = constrain(angleY, SERVO2_MIN, SERVO2_MAX);

  // Servo direction is mounted flipped
  angleY = angleY * -1;
  angleX = angleX * -1;

  // The actual values accepted by the servo are 0 to 180
  // The bucket is attached to servos such that the "real" servo
  // positions 90;90 are the middle position of the bucket (logical 0;0).
  servo1.write(angleX + 90);
  servo2.write(angleY + 90);
}
