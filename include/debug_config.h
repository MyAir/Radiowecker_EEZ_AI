#pragma once

// This centralized header file controls all debug flags
// It can be included in any source file that needs debug flags

// Check #ifndef DEBUG_CONFIG_H
#define DEBUG_CONFIG_H

// If preprocessor flag is not specified, provide defaults here

// Controls debug output for configuration loading and parsing
#ifndef CONFIG_DEBUG
  #define CONFIG_DEBUG 0
#endif

// Controls debug output for WiFi connection
#ifndef WIFI_DEBUG
  #define WIFI_DEBUG 0
#endif

// Check if UI_DEBUG is on - if so, enable all UI-related debug outputs
#if defined(UI_DEBUG) && UI_DEBUG == 1
  #ifndef STATUS_DEBUG
    #define STATUS_DEBUG 1
  #endif
  #ifndef TIME_DEBUG
    #define TIME_DEBUG 1
  #endif
  #ifndef SENSOR_DEBUG
    #define SENSOR_DEBUG 1
  #endif
  #ifndef WEATHER_DEBUG
    #define WEATHER_DEBUG 1
  #endif
#endif

// Default values if not already defined
#ifndef SD_DEBUG
  #define SD_DEBUG 0
#endif

#ifndef TOUCH_DEBUG
  #define TOUCH_DEBUG 0
#endif

#ifndef SYSTEM_DEBUG
  #define SYSTEM_DEBUG 0
#endif

#ifndef HEAP_DEBUG
  #define HEAP_DEBUG 0
#endif

#ifndef STATUS_DEBUG
  #define STATUS_DEBUG 0
#endif

#ifndef TIME_DEBUG
  #define TIME_DEBUG 0
#endif

#ifndef SENSOR_DEBUG
  #define SENSOR_DEBUG 0
#endif

#ifndef WEATHER_DEBUG
  #define WEATHER_DEBUG 0
#endif

// Controls debug output for the Alarm Manager
#ifndef ALARM_DEBUG
  #define ALARM_DEBUG 1
#endif

// Controls debug output for the Alarm Manager
#ifndef ALARM_UI_DEBUG
  #define ALARM_UI_DEBUG 1
#endif

// Dual Serial Debug Output Macros
// These send debug output to both USB CDC (Serial) and UART (Serial0)
#define DEBUG_PRINT(x) do { \
  Serial.print(x); \
  Serial0.print(x); \
} while(0)

#define DEBUG_PRINTLN(x) do { \
  Serial.println(x); \
  Serial0.println(x); \
} while(0)

#define DEBUG_PRINTF(fmt, ...) do { \
  Serial.printf(fmt, ##__VA_ARGS__); \
  Serial0.printf(fmt, ##__VA_ARGS__); \
} while(0)
