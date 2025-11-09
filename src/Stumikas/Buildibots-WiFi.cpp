
// Buildibots-WiFi.cpp
//
// Common utility for Wi-Fi setup. WiFi connection can be either in access point
// mode or client mode, see `config-wifi.h` for more info.

#include "Buildibots-WiFi.h"

#include "Arduino.h"
#include <WiFi.h>
#include "WString.h"
#include "Buildibots-Serial.h"
#include "config-wifi.h"


/**
 * @brief Initializes and configures the Wi-Fi connection for the device.
 *
 * Depending on the compile-time setting (WIFI_AP_MODE), this function either:
 * - Sets up the board as a Wi-Fi Access Point with a fixed IP, SSID, and password, or
 * - Connects the board as a Wi-Fi client to an existing network.
*
 * @return void
 */
void wifi_setup() {

  print_info("Setting up Wi-Fi...");
  

  #if WIFI_AP_MODE

  print_info("Wi-Fi is configured in Access Point mode.");

  // Configure fixed IP
  if (!WiFi.softAPConfig(WIFI_LOCAL_IP, WIFI_LOCAL_IP, IPAddress(255, 255, 255, 0))) {
    print_error("Failed to configure Access Point IP settings!");
  }

  // Start the Access Point
  bool success = WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
  if (success) {
    print_info("Access Point started successfully!");
  } else {
    print_error("Failed to start Access Point!");
    return;
  }

  print_info("Access Point SSID:     " + String(WIFI_SSID));
  print_info("Access Point password: " + String(WIFI_PASSWORD));
  print_info("Board IP address:      " + WiFi.softAPIP().toString());

  #else // WIFI_AP_MODE

  print_info("Wi-Fi is configured in Client mode.");

  // Connect to Wi-Fi
  print_info("Connecting to Wi-Fi SSID `" + String(WIFI_SSID) + "`...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while( WiFi.status() != WL_CONNECTED ) {
    delay(500);
    print_info("Waiting for connection...");
  }

  print_info("Connected successfully.");
  print_info("Board IP address: " + WiFi.localIP().toString());

  #endif

}