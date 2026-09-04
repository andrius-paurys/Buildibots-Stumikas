
// Stumikas-Screen.h
//
// Handles playback of "GIF-like" animations defined as byte arrays on an LED matrix.
// Runs in a separate task from the main loop for smooth, non-blocking updates.

#ifndef STUMIKAS_SCREEN_H
#define STUMIKAS_SCREEN_H

#include <cstdint>


// Indices into IMAGES[], named after the source GIF files.
constexpr int IMAGE_ANGRY        = 0; // angry.gif
constexpr int IMAGE_CUTE         = 1; // cute.gif
constexpr int IMAGE_FAST_FORWARD = 2; // fast-forward.gif
constexpr int IMAGE_FORWARD      = 3; // forward.gif
constexpr int IMAGE_IDLE_120     = 4; // idle-120.gif
constexpr int IMAGE_IDLE_15      = 5; // idle-15.gif
constexpr int IMAGE_IDLE_60      = 6; // idle-60.gif
constexpr int IMAGE_MAX_LEFT     = 7; // max-left.gif
constexpr int IMAGE_MAX_REVERSE  = 8; // max-reverse.gif
constexpr int IMAGE_MAX_RIGHT    = 9; // max-right.gif
constexpr int IMAGE_RELIEVED     = 10; // relieved.gif
constexpr int IMAGE_REVERSE      = 11; // reverse.gif
constexpr int IMAGE_SAD          = 12; // sad.gif
constexpr int IMAGE_STALL        = 13; // stall.gif
constexpr int IMAGE_STARTUP_LOGO = 14; // startup-logo.gif


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