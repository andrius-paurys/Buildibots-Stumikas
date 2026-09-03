
// Stumikas-Screen.h
//
// Handles playback of "GIF-like" animations defined as byte arrays on an LED matrix.
// Runs in a separate task from the main loop for smooth, non-blocking updates.

#ifndef STUMIKAS_SCREEN_H
#define STUMIKAS_SCREEN_H

#include <cstdint>


/**
 * If `switchAnimation` is a positive integer, on next frame draw, the animation will switch to
 * the image at that index, and the frame counter will be reset. If value is -1, then o animation
 * switch occurs.
*/
extern int8_t switchAnimation;


/**
 * Number of animation images available in the generated image library.
 * Valid `switchAnimation` image indices are 0 to `SCREEN_IMAGE_COUNT - 1`.
*/
extern const int8_t SCREEN_IMAGE_COUNT;


/**
 * Initialize FastLED and run screen refresh loop task.
 * @return void
 */
void screen_setup();


#endif