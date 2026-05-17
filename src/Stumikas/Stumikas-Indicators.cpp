
// Stumikas-Indicators.cpp
//
// Handles control of two indicator LEDs in the back of the bot.

#include "Stumikas-Indicators.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <FastLED.h>
#include "Buildibots-Serial.h"
#include "config-pins.h"


#define NUM_LEDS    2
#define BRIGHTNESS  150
#define LED_CHIP    WS2812
#define COLOR_MODE  GRB

// Target refresh rate for indicator color values
#define INDICATORS_TARGET_FPS 15

/**
 * Current color being displayed on both indicators.
 **/
CHSV currentColor = CHSV(0, 255, 255); // Initial color - Red


namespace {

  // LED matrix RGB value buffer
  CRGB leds[NUM_LEDS];


  /**
  * Indicator color update/render loop.
  * @return void
  */
  static void indicatorsLoop( void *pvParameters ) {

    // Initialize `xLastWakeTime` with the current time.
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xTimeIncrement  = pdMS_TO_TICKS(1000 / INDICATORS_TARGET_FPS);

    for(;;) {
      currentColor.hue += 10;

      leds[0] = currentColor;
      leds[1] = currentColor;

      FastLED.show();

      vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);
    }

  }

} // namespace


/**
 * Initialize FastLED and run task for indicator color updates.
 * @return void
 */
void indicators_setup() {
  print_info("Initializing FastLED for indicators...");
  FastLED.addLeds<LED_CHIP, INDICATORS_PIN, COLOR_MODE>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

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
