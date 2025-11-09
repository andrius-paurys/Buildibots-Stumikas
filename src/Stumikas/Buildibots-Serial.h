
// Buildibots-Serial.h
//
// Universal Utility from printing debug messages to serial.

#ifndef BUILDIBOTS_SERIAL_H
#define BUILDIBOTS_SERIAL_H

#include "WString.h"


void serial_setup(void);

void print_error(const char[]);
void print_info(const char[]);
void print_debug(const char[]);
void print_warn(const char[]);

void print_error(const String &);
void print_info(const String &);
void print_debug(const String &);
void print_warn(const String &);


#endif // BUILDIBOTS_SERIAL_H