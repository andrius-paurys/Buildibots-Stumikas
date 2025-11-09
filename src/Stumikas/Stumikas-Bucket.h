
// Stumikas-Bucket.h
//
// Provides functions for controlling the bot's loading bucket servos.

#ifndef STUMIKAS_BUCKET_H
#define STUMIKAS_BUCKET_H

#include <cstdint>


/**
 * Setup PWM output for loading bucket control.
 * @return void
 */
void bucket_setup(void);


/**
 * @brief Adjusts servo angles to control the bot's loading bucket.
 *
 * Accepts angle values from -90 to 90, relative to the bucket's centered position.
 * Values will be trimmed to respect real measured physical bucket clearances.
 *
 * @param angleX Desired horizontal angle for the bucket (-90 to 90, relative to center).
 * @param angleY Desired vertical angle for the bucket (-90 to 90, relative to center).
 */
void bucket_tick(int angleX, int angleY);


#endif // STUMIKAS_BUCKET_H