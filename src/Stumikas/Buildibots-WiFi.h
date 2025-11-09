
// Buildibots-WiFi.h
//
// Common utility for Wi-Fi setup. WiFi connection can be either hotspot or
// client, see `config-wifi.h` for more info.

#ifndef BUILDIBOTS_WIFI_H
#define BUILDIBOTS_WIFI_H


/**
 * @brief Initializes and configures the Wi-Fi connection for the device.
 *
 * Depending on the compile-time setting (WIFI_AP_MODE), this function either:
 * - Sets up the board as a Wi-Fi Access Point with a fixed IP, SSID, and password, or
 * - Connects the board as a Wi-Fi client to an existing network.
*
 * @return void
 */
void wifi_setup(void);


#endif // BUILDIBOTS_WIFI_H
