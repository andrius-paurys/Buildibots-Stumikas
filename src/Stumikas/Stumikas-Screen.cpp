
// Stumikas-Screen.cpp
//
// Handles playback of "GIF-like" animations defined as byte arrays on an LED matrix.
// Runs in a separate task from the main loop for smooth, non-blocking updates.

#include "Stumikas-Screen.h"

#include <FastLED.h>
#include "Stumikas-images.h"
#include "Buildibots-Serial.h"
#include "config-pins.h"


#define NUM_LEDS    289
#define BRIGHTNESS  30
#define LED_CHIP    WS2812
#define COLOR_MODE  GRB


namespace {

  // LED matrix RGB value buffer
  CRGB leds[NUM_LEDS];

  // Index of the currently selected animation image
  uint8_t currentImage = 0;
  // Frame index from the `currentImage` that will be rendered next
  uint8_t currentFrame = 0;

  /**
  * @brief Draw a single frame of animation to FastLED.
  * Will advance `currentFrame` value to the next frame index,
  * or will loop back to frame 0.
  *
  * @param imageIndex Index of the image to draw.
  * @param frameIndex Index of the frame to draw.
  * @return void
  */
  void drawFrame(uint8_t imageIndex, uint8_t *frameIndex) {

    print_debug("Screen drawing image index " + String(imageIndex) + ", frame index " + String(*frameIndex));

    const uint8_t (*pImage)[NUM_LEDS];
    size_t frameCount;

    // TODO: Put image data in structs that would also hold frame count
    // TODO: Store animation library as an array
    switch(imageIndex) {
      case 0:
        pImage = image_0;
        frameCount = sizeof(image_0) / sizeof(image_0[0]);
        break;
      case 1:
        pImage = image_1;
        frameCount = sizeof(image_1) / sizeof(image_1[0]);
        break;
      case 2:
        pImage = image_2;
        frameCount = sizeof(image_2) / sizeof(image_2[0]);
        break;
      case 3:
        pImage = image_3;
        frameCount = sizeof(image_3) / sizeof(image_3[0]);
        break;
      case 4:
        pImage = image_4;
        frameCount = sizeof(image_4) / sizeof(image_4[0]);
        break;
      case 5:
        pImage = image_5;
        frameCount = sizeof(image_5) / sizeof(image_5[0]);
        break;
    }

    for(size_t i=0; i < NUM_LEDS; i++) {
      leds[i] = palette[ pImage[*frameIndex][i] ];
    }

    FastLED.show();

    (*frameIndex)++;
    if (*frameIndex >= frameCount) { *frameIndex = 0; }
  }
} // namespace


/**
 * Initialize FastLED.
 * @return void
 */
void screen_setup() {
  print_info("Initializing FastLED...");
  FastLED.addLeds<LED_CHIP, LED_PIN, COLOR_MODE>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
}


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
void screen_tick(int8_t* pNextAnim) {

    // If a request to switch to some other animation is recorded, it is consumed here.
    if (*pNextAnim > -1) {
      currentImage = *pNextAnim;
      *pNextAnim = -1;
      currentFrame = 0;
    }

    drawFrame(currentImage, &currentFrame);
}
