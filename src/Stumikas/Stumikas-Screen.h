
// Stumikas-Screen.h
//
// Handles playback of "GIF-like" animations defined as byte arrays on an LED matrix.
// Runs in a separate task from the main loop for smooth, non-blocking updates.

#ifndef STUMIKAS_SCREEN_H
#define STUMIKAS_SCREEN_H

#include <cstdint>


/**
 * Initialize FastLED.
 * @return void
 */
void screen_setup();


/**
 * @brief Draws a single frame of the currently active animation.
 *
 * If `nextAnim` is a positive integer, the animation will switch to
 * the image at that index, and the frame counter will be reset.
 *
 * @param nextAnim Pointer to an int8_t specifying the next animation index (optional).
 *                 If value is `-1`, no animation switch occurs.
 * @return void
 */
void screen_tick(int8_t* nextAnim);


#endif