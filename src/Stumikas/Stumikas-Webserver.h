
// Stumikas-Webserver.h
//
// Buildibots Stumikas Wi-Fi and Webserver for remote control.

#ifndef STUMIKAS_WEBSERVER_H
#define STUMIKAS_WEBSERVER_H

#include <cstdint>


/**
 * Setup web-server for bot remote control.
 * 
 * Webserver provides a simple API to control the bot. Two endpoints provided:
 *  /updateState   - for sending a new bot state from the remote controller
 *  /nextAnimation - changes the current animation displayed on the LED matrix
 *
 * Webserver provides a simple static JavaScript joystick from LittleFS at path '/'.
 * See `PREREQUISITES.md` for information on how to upload it to the ESP.
 *
 * @param speed Pointer to where the webserver will write the updated bot speed.
 * @param turn Pointer to where it will write the updated turn value.
 * @param bucketX Pointer to where it will write the updated bucket X angle.
 * @param bucketY Pointer to where it will write the updated bucket Y angle.
 * @param nextAnim Pointer to where it will write the request to skip animation.
 * @return void
 */
void webserver_setup(int* speed, int* turn, int* bucketX, int* bucketY, int8_t* nextAnim);


#endif // STUMIKAS_WEBSERVER_H