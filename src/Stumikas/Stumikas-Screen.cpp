
// Stumikas-Screen.cpp
//
// Handles playback of "GIF-like" animations defined as byte arrays on an LED matrix.
// Runs in a separate task from the main loop for smooth, non-blocking updates.

#include "Stumikas-Screen.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <FastLED.h>
#include "Stumikas-images.h"
#include "Buildibots-Serial.h"
#include "config-pins.h"


#define NUM_LEDS    289
#define BRIGHTNESS  30
#define LED_CHIP    WS2812
#define COLOR_MODE  GRB

// Target frames per second for the LED matrix screen
#define SCREEN_TARGET_FPS 10

// Animation image index that is first shown
#define STARTUP_IMAGE_INDEX 1


/**
 * If `switchAnimation` is a positive integer, on next frame draw, the animation will switch to
 * the image at that index, and the frame counter will be reset. If value is -1, then o animation
 * switch occurs.
*/
int8_t switchAnimation = STARTUP_IMAGE_INDEX;


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


  /**
  * LED matrix screen render loop.
  * @return void
  */
  static void screenLoop( void *pvParameters ) {

    // Initialize `xLastWakeTime` with the current time.
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xTimeIncrement  = pdMS_TO_TICKS(1000 / SCREEN_TARGET_FPS);

    for(;;) {
      // If a request to switch to some other animation is recorded, it is consumed here.
      if (switchAnimation > -1) {
        currentImage = switchAnimation;
        switchAnimation = -1;
        currentFrame = 0;
      }

      drawFrame(currentImage, &currentFrame);
      vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);
    }

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

  // Run the LED matrix render loop in a separate task, so it
  // does not block other workloads from CPU time.
  xTaskCreate(
      screenLoop,         // Task function
      "screenLoop",       // Task name (for debugging)
      4096,               // Stack size in words
      NULL,               // Task parameters
      tskIDLE_PRIORITY+3, // Task priority
      NULL                // Task handle
  );
  print_info("LED matrix screen render task launched.");
}
