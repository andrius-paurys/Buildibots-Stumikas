
// Buildibots-Serial.cpp
//
// Universal Utility from printing debug messages to serial.

#include "Buildibots-Serial.h"

#include "Arduino.h"
#include "HardwareSerial.h"


// Enables debug messages and periodically prints the FreeRTOS task list.
#define BUILDIBOTS_DEBUG 0

// Uncomment to force serial output to UART0
//#define Serial Serial0

#define SERIAL_BAUD_RATE 115200


#if BUILDIBOTS_DEBUG

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#define TASKLIST_BUFFER_SIZE 1024
#define TASKLIST_LOOP_WAIT 5000 // miliseconds
#define TASKLIST_STACK_SIZE TASKLIST_BUFFER_SIZE+2048

namespace {

  void loopTaskList(void *pvParameters) {
    char buffer[TASKLIST_BUFFER_SIZE];

    for(;;) {
      vTaskDelay(pdMS_TO_TICKS(TASKLIST_LOOP_WAIT));

      print_debug("FreeRTOS task list:");
      vTaskList(buffer);
      Serial.println("Task Name\tState\tPrio\tStack\t#");
      Serial.println(buffer);
    }
  }

  void setup_taskList() {
    xTaskCreate(
        loopTaskList,             // Task function
        "loopTaskList",           // Task name (for debugging)
        TASKLIST_STACK_SIZE,      // Stack size in words
        NULL,                     // Task parameters
        tskIDLE_PRIORITY+1,       // Task priority - using lowest possible
        NULL                      // Task handle
    );
  }

} // namespace

#endif // BUILDIBOTS_DEBUG


void serial_setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  print_info("Hello world!");

  #if BUILDIBOTS_DEBUG
  setup_taskList();
  #endif // BUILDIBOTS_DEBUG
}

void print_error(const char s[]) {
  Serial.print("[ERROR] ");
  Serial.println(s);
}

void print_info(const char s[]) {
  Serial.print("[INFO]  ");
  Serial.println(s);
}

void print_debug(const char s[]) {
  #if BUILDIBOTS_DEBUG
  Serial.print("[DEBUG] ");
  Serial.println(s);
  #endif // BUILDIBOTS_DEBUG
}

void print_warn(const char s[]) {
  Serial.print("[WARN]  ");
  Serial.println(s);
}

void print_error(const String &s) { print_error(s.c_str()); }
void print_info(const String &s) { print_info(s.c_str()); }
void print_debug(const String &s) { print_debug(s.c_str()); }
void print_warn(const String &s) { print_warn(s.c_str()); }
