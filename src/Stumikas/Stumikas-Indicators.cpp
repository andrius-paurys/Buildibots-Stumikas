
// Stumikas-Indicators.cpp
//
// Handles control of two indicator LEDs in the back of the bot.

#include "Stumikas-Indicators.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <FastLED.h>
#include "Buildibots-Serial.h"
#include "config-pins.h"


#define NUM_LEDS     2
#define BRIGHTNESS_I 200
#define LED_CHIP     WS2812
#define COLOR_MODE   GRB

// Target refresh rate for indicator color values
#define INDICATORS_TARGET_FPS 15

/**
 * Current color being displayed on both indicators.
 **/
CHSV currentColor = CHSV(0, 255, 255); // Initial color - Red


namespace {

  // LED matrix RGB value buffer
  CRGB leds[NUM_LEDS];

  // Pointer to where the bot speed is stored
  int *pSpeed;


  /**
  * Indicator color update/render loop.
  * @return void
  */
  static void indicatorsLoop( void *pvParameters ) {

    // Initialize `xLastWakeTime` with the current time.
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xTimeIncrement  = pdMS_TO_TICKS(1000 / INDICATORS_TARGET_FPS);

    for(;;) {

      // Cycle hue at a speed proportinal to bot motor power
      if(*pSpeed > 0) {
        currentColor.hue += *pSpeed / 15;
      }

      // Red when stopped
      if(*pSpeed == 0) {
        //currentColor.hue = 0;
      }

      // Reset saturation after reversing
      if(*pSpeed >= 0) {
        currentColor.sat = 255;
      }

      // Full white when reversing
      if(*pSpeed < 0) {
        currentColor.hue = 0;
        currentColor.sat = 0;
      }

      leds[0] = currentColor;
      leds[1] = currentColor;

      // Apply brightness
      leds[0].nscale8_video(BRIGHTNESS_I);
      leds[1].nscale8_video(BRIGHTNESS_I);

      FastLED.show();

      vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);
    }

  }

} // namespace


/**
 * Initialize FastLED and run task for indicator color updates.

 * @param speed Pointer to where should indicators read the speed of the bot.
 * @return void
 */
void indicators_setup(int* speed) {
  print_info("Initializing FastLED for indicators...");
  FastLED.addLeds<LED_CHIP, INDICATORS_PIN, COLOR_MODE>(leds, NUM_LEDS);

  pSpeed = speed;

  // Run the indicator render loop in a separate task, so it
  // does not block other workloads from CPU time.
  xTaskCreate(
      indicatorsLoop,     // Task function
      "indicatorsLoop",   // Task name (for debugging)
      4096,               // Stack size in words
      NULL,               // Task parameters
      tskIDLE_PRIORITY+3, // Task priority
      NULL                // Task handle
  );
  print_info("Indicators color update task launched.");
}
