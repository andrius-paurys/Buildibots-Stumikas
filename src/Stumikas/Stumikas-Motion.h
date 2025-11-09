
// Stumikas-Motion.h
//
// Provides functions for controlling the bot's drive DC motors and steering.

#ifndef STUMIKAS_MOTION_H
#define STUMIKAS_MOTION_H


/**
 * Setup DC motors for motion and turn control.
 * @return void
 */
void motion_setup(void);


/**
 * Adjusts motor speeds to control the robot's forward motion and turning rate.
 *
 * @param speed Movement speed and direction, -255 (back) to 255 (forward).
 * @param turn Turn speed, -255 (left) to 255 (right).
 * @return void
 */
void motion_tick(int speed, int turn);


#endif // STUMIKAS_MOTION_H