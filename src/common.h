#ifndef PSN_COMMON_H
#define PSN_COMMON_H

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#include <SoftwareSerial.h>
#include "config.h"
#include "util.h"

extern SoftwareSerial g_soft_serial;

#endif
