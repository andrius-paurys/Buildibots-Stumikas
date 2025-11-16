
// Stumikas.ino
//
// Buildibot Stumikas

#include "Arduino.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "Buildibots-Serial.h"
#include "Buildibots-WiFi.h"
#include "Stumikas-Screen.h"
#include "Stumikas-Motion.h"
#include "Stumikas-Bucket.h"
#include "Stumikas-Webserver.h"


// Animation image index that is first shown
#define STARTUP_IMAGE_INDEX 1

// Time delay (ms) between successive actuator ticks (updates).
// Don't set this too low as it might starve other tasks of CPU time.
#define ACTUATOR_LOOP_DELAY 30

// Target frames per second for the LED matrix screen
#define SCREEN_TARGET_FPS 10


// Variables to store the bot state
static int speed = 0;   // -255 backward, 0 stop, 255 forward
static int turn = 0;    // -255 left, 0 straight, 255 right
static int bucketX = 0;  // -90 to 90
static int bucketY = 0;  // -90 to 90

// Request to switch to a certain animation. -1 means no switch.
static int8_t nextAnim = STARTUP_IMAGE_INDEX;


/**
 * Bot actuator control loop.
 * @return void
 */
static void actuatorsLoop( void *pvParameters ) {
  for(;;) {
    motion_tick(speed, turn);
    bucket_tick(bucketX, bucketY);
    vTaskDelay(pdMS_TO_TICKS(ACTUATOR_LOOP_DELAY));
  }
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
    screen_tick(&nextAnim);
    vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);
  }

}


/**
 * Arduino setup entrypoint. Initializes everything and creates loop tasks.
 * @return void
 */
void setup() {
  serial_setup();
  screen_setup();

  // Run the LED matrix render loop in a separate task, so it
  // does not block other workloads from CPU time.
  xTaskCreate(
      screenLoop,         // Task function
      "screenLoop",       // Task name (for debugging)
      4096,               // Stack size in words
      NULL,               // Task parameters
      tskIDLE_PRIORITY+3, // Task priority - using lowest possible, screen not important
      NULL                // Task handle
  );
  print_info("LED matrix screen render task launched.");

  motion_setup();
  bucket_setup();

  // Run the actuator loop in its own FreeRTOS task.
  // Task is a FreeRTOS primitive that resembles a process.
  xTaskCreate(
      actuatorsLoop,      // Task function
      "actuatorsLoop",    // Task name (for debugging)
      4096,               // Stack size in words
      NULL,               // Task parameters
      tskIDLE_PRIORITY+2, // Task priority
      NULL                // Task handle
  );
  print_info("Actuator control loop task launched.");

  // TODO Get the stack sizes right. For ESP IDF it might actually be defined in bytes.
  // See: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos_idf.html

  wifi_setup();
  webserver_setup(&speed, &turn, &bucketX, &bucketY, &nextAnim);
}


/**
 * @brief Main Arduino task loop. Intentionally empty.
 *
 * It is not being used because it runs under very low priority
 * and has issues with use of `vTaskDelay()`.
 *
 * @return void
 */
void loop() { }
