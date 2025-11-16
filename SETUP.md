# Buildibots Stumikas Build Setup

This guide provides the necessary steps to set up your environment, install dependencies, and
upload the firmware and filesystem to the Axiometa ESP32 board.


## 1. Prerequisites

### 🧩 Software

* **Arduino IDE:** Download and install the latest version from
  [the official website](https://www.arduino.cc/en/software/).
* **Arduino CLI (Optional):** A command-line tool for advanced users. Install from
  [here](https://docs.arduino.cc/arduino-cli/installation/).
    * **Windows Tip:** You can place the `arduino-cli.exe` in your Arduino IDE installation
      folder (e.g., `C:\Program Files\Arduino IDE`) and add this directory to your system's
      `%PATH%` environment variable for easy access.



## 2. Environment Setup

### ⚙️ Installing ESP32 Board Support

You need to add support for ESP32 boards to your Arduino environment.


**Using the Arduino IDE (UI):**

>  1.   Open the Arduino IDE.
>  2.   Go to **File > Preferences** (or **Arduino IDE > Settings...** on macOS).
>  3.   In the **Additional Board Manager URLs** field, add the following URL:
>       ```
>       https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
>       ```
>  5.   Click **OK**.
>  6.   Go to **Tools > Board > Boards Manager...**.
>  7.   Search for "esp32".
>  8.   Find "esp32 by Espressif Systems" and click **Install**. Last tested with version _3.3.2_.


**Using the Arduino CLI:**

>  Run the following commands to add the URL and install the core:
>  
>  ```bash
>  # Add the ESP32 board manager URL
>  arduino-cli core update-index --additional-urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
>  
>  # Install the ESP32 core
>  arduino-cli core install esp32:esp32
>  ```


---
### 📦 Installing Library Dependencies

The firmware depends on specific libraries. Note: The versions listed are the ones last tested
with the firmware.


*  Async TCP (_v3.4.9_)
*  ESP Async WebServer (_v3.8.1_)
*  ESP32Servo (_v3.0.9_)
*  FastLED (_v3.10.3_)
*  LittleFS_esp32 (_v1.0.6_)


**Using Arduino IDE (UI):**

>  1.  Go to **Tools > Manage Libraries...**
>  2.  Search for and install each of the libraries.


**Using Arduino CLI:**

>  Run the following commands.
>  
>  ```bash
>  # Install libraries from the standard registry
>  arduino-cli lib install --no-overwrite "Async TCP"@3.4.9
>  arduino-cli lib install --no-overwrite "ESP Async WebServer"@3.8.1
>  arduino-cli lib install --no-overwrite "ESP32Servo"@3.0.9
>  arduino-cli lib install --no-overwrite "FastLED"@3.10.3
>  arduino-cli lib install --no-overwrite "LittleFS_esp32"@1.0.6
>  ```


---
### 📁 Installing LittleFS Upload Tool

To upload the static web files (HTML, JS, CSS) to the ESP32's filesystem, you need a plugin.

1.  Go to the [arduino-littlefs-upload](https://github.com/earlephilhower/arduino-littlefs-upload)
    releases page.
2.  Download the latest VSIX file (e.g. `arduino-littlefs-upload-1.5.5.vsix`).
3.  Copy the VSIX file to `~/.arduinoIDE/plugins/` on Mac and Linux or
    `C:\Users\<username>\.arduinoIDE\plugins\` on Windows.
4.  Restart Arduino IDE.
5.  For more information consult the
    [README](https://github.com/earlephilhower/arduino-littlefs-upload?tab=readme-ov-file#installation).


---
## 3. Configuration & Upload

First, open the sketch (`Stumikas.ino` file) in Arduino IDE. Then, perform the following.


### ⚙️ Selecting Target Board

Select the correct board before compiling and uploading.

*  **Arduino IDE:** Go to **Tools > Board > esp32 > Axiometa PIXIE M1**.
*  **Arduino CLI:** Use the Fully Qualified Board Name (FQBN)
   `esp32:esp32:axiometa_pixie_m1` in your commands.


### 🛠️ Configuring the Firmware

Before compiling, edit the configuration files to match your setup:

*  `config-wifi.h`: Set your Wi-Fi network's SSID and password.
*  `config-pins.h`: Verify the pin definitions match your wiring.

> ℹ️ You **do not need to configure Wi-Fi** for the bot to be usable. If `config-wifi.h` is left
> with the default settings, the ESP32 will automatically start in **Access Point (AP) mode**.
> The AP will be named _Buildibots Stumikas_. When you connect a device, a captive portal
> with a web-based joystick will automatically open in your browser. If not - you can access the
> controls using the default URL [http://4.3.2.1/](http://4.3.2.1/) in your browser.


### 🚀 Compiling and Uploading the Firmware

**Arduino IDE:**

1.  Select the correct **Port** from the **Tools** menu.
2.  (Optional) In the **Tools** menu select these options:
    *  Desired **Core Debug Level**;
    *  Set **Erase All Flash Before Sketch Upload** to **Enabled**;
    *  Set **Partition Scheme** to **Minimal SPIFFS**.
3.  Click the **Upload** button (the arrow icon). This will compile and upload the sketch in
    one step.


### 📁 Uploading Filesystem (LittleFS) ###

The `ESPAsyncWebServer` serves static files from the ESP32's internal flash memory. These files
must be uploaded separately.

1.  First **Build and Upload** the sketch (as described above).
2.  With the Arduino sketch till open, press **Ctrl+Shift+P** and search for **Upload LittleFS
    to Pico/ESP8266/ESP32** to start the upload.

This will build a LittleFS filesystem image from the `data` folder and upload it to the board.



## 4. Web Interface and API Control

The firmware includes a built-in **web-based joystick** interface for remote control.

Once the ESP32 connects to your Wi-Fi network, you can access the control panel in your web
browser, e.g.: [http://4.3.2.1/](http://4.3.2.1/).

> 💡 Replace the default `4.3.2.1` with the actual IP address assigned to the ESP32, which
> you can configure in `config-wifi.h` or view via the Serial Monitor on startup.

You can also send direct control commands to the bot programmatically using HTTP POST
requests, e.g. using cURL:

```
curl -X POST http://4.3.2.1/updateState -d "speed=0" -d "turn=0" -d "bucketX=0" -d "bucketY=0"
````


_Enjoy your build!_

