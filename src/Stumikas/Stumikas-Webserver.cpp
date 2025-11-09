
// Stumikas-Webserver.cpp
//
// Buildibots Stumikas Wi-Fi and Webserver for remote control.

#include "Stumikas-Webserver.h"

#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include "WString.h"
#include "Buildibots-Serial.h"
#include "config-wifi.h"


namespace {

  // Pointers to variables where the web-server will write an updated bot state.
  // The variables that store the actual value are defined in `Stumikas.ino`.
  int *pSpeed;
  int *pTurn;
  int *pBucketX;
  int *pBucketY;
  int8_t *pNextAnim;

  // DNS server for Wi-Fi captive portal
  #if WIFI_AP_MODE
  DNSServer dnsServer;
  #endif

  // Create AsyncWebServer on port 80
  AsyncWebServer server(80);


  /**
  * Handle bot state update HTTP POST.
  *
  * Record speed, turn and bucket position to shared memory. Respective actuator control
  * code loops will pick up the updated values.
  *
  * @param request HTTP request data.
  * @return void
  */
  void handle_updateState(AsyncWebServerRequest *request) {
    // At first, if changed, the new values will be stored locally.
    int speed = *pSpeed;
    int turn = *pTurn;
    int bucketX = *pBucketX;
    int bucketY = *pBucketY;
    int8_t nextAnim = -1;

    // Read motor speed/turn values
    if(request->hasParam("speed", true)) {
      speed = request->getParam("speed", true)->value().toInt();
      speed = constrain(speed, -255, 255);  // ensure valid range

      if ( speed < -150 ) { nextAnim = 0; }
    }
    if(request->hasParam("turn", true)) {
      turn = request->getParam("turn", true)->value().toInt();
      turn = constrain(turn, -255, 255); // ensure valid range

      if ( turn > 150 ) { nextAnim = 3; }
    }

    // Read bucket angles
    if(request->hasParam("bucketX", true)) {
      bucketX = request->getParam("bucketX", true)->value().toInt();
      bucketX = constrain(bucketX, -90, 90);  // ensure valid range
    }
    if(request->hasParam("bucketY", true)) {
      bucketY = request->getParam("bucketY", true)->value().toInt();
      bucketY = constrain(bucketY, -90, 90);  // ensure valid range
    }

    // Write the values back to the main shared scope
    *pSpeed = speed;
    *pTurn = turn;
    *pBucketX = bucketX;
    *pBucketY = bucketY;
    if ( nextAnim > -1 ) {
      *pNextAnim = nextAnim;
    }
    
    char message[80];
    sprintf(message, "API call: state updated. Speed:%d Turn:%d BucketX:%d BucketY:%d.", speed, turn, bucketX, bucketY);
    print_info(message);

    // Respond HTTP OK
    request->send(200, "text/plain", message);
  }


  /**
  * The animation index from the last "next animation" request is
  * stored, so multiple clicks continue the sequence.
  */
  int8_t lastAnimSwitch = -1;

  /**
  * Forces a skip to the next animation in the loop.
  *
  * @param request HTTP request data.
  * @return void
  */
  void handle_nextAnimation(AsyncWebServerRequest *request) {
      // Can't just do `*pNextAnim++` because the animation might have already switched to
      // something out of sequence since the last call to `handle_nextAnimation`.
      lastAnimSwitch++;

      // TODO Fix hardcoded max image count
      if ( lastAnimSwitch > 5 ) { lastAnimSwitch = 0; }
      *pNextAnim = lastAnimSwitch;

      print_info("API call: animation switched.");
      // Respond HTTP OK
      request->send(200, "text/plain", "Animation switched.");
  }


  /**
  * Setup LittleFS mount for retrieving static files.
  * @return void
  */
  void setup_littlefs() {
    print_info("Mounting LittleFS...");
    bool result = LittleFS.begin(false); // formatOnFail = false

    if (!result) {
      print_error("LittleFS mount failed.");
    }
    else {
      print_info("Successfully mounted LittleFS at `" + String(LittleFS.mountpoint()) + "`.");
    }
  }


  #if WIFI_AP_MODE
  /**
  * Setup Wi-Fi captive portal. Enable DNS server and register popular 
  */
  void setup_captivePortal(void) {

    print_info("Starting the DNS server...");

    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    if (!dnsServer.start(53, "*", WIFI_LOCAL_IP)) {
      print_error("Error starting DNS server for the Wi-Fi Captive Portal.");
    }

    String baseUrl = "http://" + (WIFI_LOCAL_IP.toString()) + "/";


    // Android connectivity check path - respond with redirect (makes Android open captive portal)
    server.on("/generate_204", HTTP_GET, [baseUrl](AsyncWebServerRequest *request){
      // Android expects a 204 for "no captive portal". If we redirect, Android shows portal.
      request->redirect(baseUrl.c_str());
    });

    // iOS checks
    server.on("/hotspot-detect.html", HTTP_GET, [baseUrl](AsyncWebServerRequest *request){
      request->redirect(baseUrl.c_str());
    });
    server.on("/library/test/success.html", HTTP_GET, [baseUrl](AsyncWebServerRequest *request){
      request->redirect(baseUrl.c_str());
    });

    // Windows chekcs
    server.on("/connecttest.txt", HTTP_GET, [baseUrl](AsyncWebServerRequest *request){
      request->redirect(baseUrl.c_str());
    });
    server.on("/ncsi.txt", HTTP_GET, [baseUrl](AsyncWebServerRequest *request){
      request->redirect(baseUrl.c_str());
    });

    // KDE NetworkManager
    server.on("/check_network_status.txt", HTTP_GET, [baseUrl](AsyncWebServerRequest *request){
      request->redirect(baseUrl.c_str());
    });
  }

  #endif
} // namespace


/**
 * Setup web-server for bot remote control.
 * 
 * Webserver provides a simple API to control the bot. Two endpoints provided:
 *  /updateState   - for sending a new bot state from the remote controller
 *  /nextAnimation - changes the current animation displayed on the LED matrix
 *
 * Webserver provides a simple static JavaScript joystick from LittleFS at path '/'.
 * See `SETUP.md` for information on how to upload it to the ESP.
 *
 * @param speed Pointer to where the web-server will write the updated bot speed.
 * @param turn Pointer to where it will write the updated turn value.
 * @param bucketX Pointer to where it will write the updated bucket X angle.
 * @param bucketY Pointer to where it will write the updated bucket Y angle.
 * @param nextAnim Pointer to where it will write the request to skip animation.
 * @return void
 */
void webserver_setup(int* speed, int* turn, int* bucketX, int* bucketY, int8_t* nextAnim) {

  // Store the pointers. The web-server will write to these addresses asynchronously.
  pSpeed = speed;
  pTurn = turn;
  pBucketX = bucketX;
  pBucketY = bucketY;
  pNextAnim = nextAnim;

  // Register bot control API endpoint events
  server.on("/updateState", HTTP_POST, handle_updateState);
  server.on("/nextAnimation", HTTP_POST, handle_nextAnimation);

  // Read static artifacts from LittleFS
  setup_littlefs();
  // Serve static HTML with JavaScript joystick and API client.
  server.serveStatic("/", LittleFS, "/").setDefaultFile("joystick.html");

  // Start the HTTP server
  print_info("Starting the HTTP server...");
  server.begin();

  // Start the DNS server, attach redirect events for Wi-Fi captive portal
  #if WIFI_AP_MODE
  setup_captivePortal();
  #endif

  print_info("Web-server ready and should be accepting connections.");
}