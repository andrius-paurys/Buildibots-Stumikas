
// config-wifi.h
//
// Configures WiFi hotspot / client settings for remote control.

#pragma once


// Toogle Wi-Fi Access Point and Client modes. Set to 1 for AP, or 0 for Client.
#define WIFI_AP_MODE    1

// Replace with your Wi-Fi credentials.
#define WIFI_SSID       "Buildibots Stumikas"
#define WIFI_PASSWORD   ""

// Fixed IP configuration for the Access Point mode
#define WIFI_LOCAL_IP   IPAddress(4, 3, 2, 1)