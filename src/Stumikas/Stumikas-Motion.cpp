
// Stumikas-Motion.cpp
//
// Provides functions for controlling the bot's drive DC motors and steering.

#include "Stumikas-Motion.h"

#include <Arduino.h>
#include "config-pins.h"


// Minimal speed so the motor doesn't suffer
#define MIN_SPEED 70


namespace {

  /**
  * Controls specific motor direction and speed by setting PWM signals on the given pins.
  *
  * @param pinA Control pin A.
  * @param pinB Control pin B.
  * @param speed Motor power level (-255 to 255). Negative values reverse direction.
  */
  static void set_motor_speed(uint8_t pinA, uint8_t pinB, int speed) {

    speed = constrain(speed, -255, 255);

    if (speed == 0) {
      analogWrite(pinA, 0);
      analogWrite(pinB, 0);
      // TODO The pins don't go LOW after digitalWrite(), maybe because they're in PWM mode
      //digitalWrite(pinA, LOW);
      //digitalWrite(pinB, LOW);
    }
    else if (speed > 0) {
      speed = max(MIN_SPEED, speed);

      analogWrite(pinB, 0);
      //digitalWrite(pinB, LOW);
      analogWrite(pinA, speed);
    }
    else if (speed < 0) {
      speed = max(MIN_SPEED, -speed);

      analogWrite(pinA, 0);
      //digitalWrite(pinA, LOW);
      analogWrite(pinB, speed);
    }

  }

} // namespace


/**
 * Setup DC motors for motion and turn control.
 * @return void
 */
void motion_setup() {
  pinMode(MOTOR_ENABLE_PIN, OUTPUT);
  digitalWrite(MOTOR_ENABLE_PIN, LOW);

  pinMode(MOTOR_LEFT_PIN1, OUTPUT);
  pinMode(MOTOR_LEFT_PIN2, OUTPUT);
  pinMode(MOTOR_RIGHT_PIN1, OUTPUT);
  pinMode(MOTOR_RIGHT_PIN2, OUTPUT);
}


/**
 * Adjusts motor speeds to control the robot's forward motion and turning rate.
 *
 * @param speed Movement speed and direction, -255 (back) to 255 (forward).
 * @param turn Turn speed, -255 (left) to 255 (right).
 * @return void
 */
void motion_tick(int speed, int turn) {

  speed = constrain(speed, -255, 255);
  turn = constrain(turn, -255, 255);

  int speed_left = constrain(speed+turn, -255, 255);
  int speed_right = constrain(speed-turn, -255, 255);

  // Disable chip if all motors are supposed to be stopped
  if (speed_left == 0 && speed_right == 0) {
    digitalWrite(MOTOR_ENABLE_PIN, LOW);
  }

  // Send motor speeds as PWM to mottor controller
  set_motor_speed(MOTOR_LEFT_PIN1, MOTOR_LEFT_PIN2, -speed_left);
  set_motor_speed(MOTOR_RIGHT_PIN1, MOTOR_RIGHT_PIN2, -speed_right);

  // Enable chip if any motor is supposed to be running
  if (speed_left != 0 || speed_right != 0) {
    digitalWrite(MOTOR_ENABLE_PIN, HIGH);
  }

}
