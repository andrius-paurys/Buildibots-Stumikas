
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


// Number of LEDs on the matrix.
#define NUM_LEDS    NUM_PIXELS
#define BRIGHTNESS  30
#define LED_CHIP    WS2812
#define COLOR_MODE  GRB

// Target frames per second for the LED matrix screen
#define SCREEN_TARGET_FPS 10

// How long each animation is shown for while cycling automatically after startup
#define SCREEN_AUTO_CYCLE_SECONDS 6

// Animation image index that is first shown (`startup_logo` in the generated image table)
#define STARTUP_IMAGE_INDEX IMAGE_STARTUP_LOGO

static_assert(STARTUP_IMAGE_INDEX < NUM_IMAGES, "STARTUP_IMAGE_INDEX is out of range");
static_assert(NUM_IMAGES > 1, "Auto-cycling needs at least one image besides the startup logo");
static_assert(NUM_IMAGES <= INT8_MAX, "Image count no longer fits the int8_t image index");


/**
 * If `switchAnimation` is a positive integer, on next frame draw, the animation will switch to
 * the image at that index, and the frame counter will be reset. If value is -1, then o animation
 * switch occurs.
*/
int8_t switchAnimation = STARTUP_IMAGE_INDEX;

/**
 * Number of animation images available in the generated image library.
 * Valid `switchAnimation` image indices are 0 to `SCREEN_IMAGE_COUNT - 1`.
*/
extern const int8_t SCREEN_IMAGE_COUNT = NUM_IMAGES;

/**
 * While true, the screen advances to the next animation image on its own every
 * `SCREEN_AUTO_CYCLE_SECONDS`, alternating the startup logo with one image from
 * the library, to demo the animations after startup. Cleared once the bot is
 * actually being controlled, so that the screen only shows what the controller asks for.
*/
volatile bool screenAutoCycle = true;


namespace {

  // LED matrix RGB value buffer
  CRGB leds[NUM_LEDS];

  // Index of the currently selected animation image
  uint8_t currentImage = 0;
  // Frame index from the `currentImage` that will be rendered next
  uint16_t currentFrame = 0;
  // Library image to show after the next startup logo repeat, while auto-cycling
  uint8_t autoCycleImage = (STARTUP_IMAGE_INDEX == 0) ? 1 : 0;

  /**
  * @brief Draw a single frame of animation to FastLED.
  * Will advance `currentFrame` value to the next frame index,
  * or will loop back to frame 0.
  *
  * Pixel data comes from the auto-generated `IMAGES` table, where every frame
  * is an array of `PALETTE` colour indices, one per LED.
  *
  * @param imageIndex Index of the image to draw.
  * @param frameIndex Index of the frame to draw.
  * @return void
  */
  void drawFrame(uint8_t imageIndex, uint16_t *frameIndex) {

    print_debug("Screen drawing image index " + String(imageIndex) + ", frame index " + String(*frameIndex));

    if (imageIndex >= NUM_IMAGES) { return; }

    const Image &image = IMAGES[imageIndex];
    if (image.frameCount == 0) { return; }

    // An out of range frame index can only happen if the image was swapped
    // between calls, so restart the animation from the beginning.
    if (*frameIndex >= image.frameCount) { *frameIndex = 0; }

    const PixelIndex *pFrame = image.frames[*frameIndex];

    for(size_t i=0; i < NUM_LEDS; i++) {
      leds[i] = PALETTE[ pFrame[i] ];
      // Apply brightness
      leds[i].nscale8_video(BRIGHTNESS);
    }

    FastLED.show();

    (*frameIndex)++;
    if (*frameIndex >= image.frameCount) { *frameIndex = 0; }
  }


  /**
  * LED matrix screen render loop.
  * @return void
  */
  static void screenLoop( void *pvParameters ) {

    // Initialize `xLastWakeTime` with the current time.
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xTimeIncrement  = pdMS_TO_TICKS(1000 / SCREEN_TARGET_FPS);

    // Time the currently shown animation was selected by automatic cycling.
    TickType_t xLastAutoSwitch = xLastWakeTime;
    const TickType_t xAutoCyclePeriod = pdMS_TO_TICKS(SCREEN_AUTO_CYCLE_SECONDS * 1000);

    for(;;) {
      
      // If a request to switch to some other animation is recorded, it is consumed here.
      if (switchAnimation > -1) {
        if (switchAnimation < NUM_IMAGES) {
          currentImage = switchAnimation;
          currentFrame = 0;
        } else {
          print_error("Requested animation index " + String(switchAnimation) + " does not exist.");
        }
        switchAnimation = -1;
      }

      // Until the bot is controlled for the first time, walk through the animation
      // library, showing the startup logo in between every image.
      else if (screenAutoCycle && (xLastWakeTime - xLastAutoSwitch) >= xAutoCyclePeriod) {
        if (currentImage == STARTUP_IMAGE_INDEX) {
          currentImage = autoCycleImage;

          // Queue up the image to show after the next logo repeat.
          do {
            autoCycleImage = (autoCycleImage + 1) % NUM_IMAGES;
          } while (autoCycleImage == STARTUP_IMAGE_INDEX);
        } else {
          currentImage = STARTUP_IMAGE_INDEX;
        }

        currentFrame = 0;
        xLastAutoSwitch = xLastWakeTime;
      }

      drawFrame(currentImage, &currentFrame);
      vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);
    }

  }

} // namespace


/**
 * Initialize FastLED and run screen refresh loop task.
 * @return void
 */
void screen_setup() {
  print_info("Initializing FastLED...");
  FastLED.addLeds<LED_CHIP, LED_PIN, COLOR_MODE>(leds, NUM_LEDS);

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
