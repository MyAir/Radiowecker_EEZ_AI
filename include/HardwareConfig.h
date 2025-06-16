#pragma once

#include <Wire.h>

// I2C Bus Configuration
#define I2C_SDA_PIN 17
#define I2C_SCL_PIN 18
#define I2C_FREQUENCY 100000  // 100kHz

// SD Card pins
#define SD_SCK  12
#define SD_MISO 13
#define SD_MOSI 11
#define SD_CS   10

// LVGL filesystem drive letter
#define DRIVE_LETTER 'S'

// Function to safely initialize/reinitialize I2C with consistent parameters
inline void initializeI2CBus() {
    Wire.end();
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, I2C_FREQUENCY);
    delay(10); // Short delay after I2C reinitialization
}

// Function to reinitialize I2C bus only if there's an error
// Returns true if bus was reinitialized, false otherwise
inline bool reinitializeI2CBusIfNeeded() {
    // Check if there's an error by trying a simple I2C transaction
    Wire.beginTransmission(0x00);
    byte error = Wire.endTransmission();
    
    // If there's an error (anything but 0=success or 2=address NACK which is expected for addr 0)
    if (error != 0 && error != 2) {
#ifdef SENSOR_DEBUG
        Serial.printf("I2C bus error detected (code %d), reinitializing...\n", error);
#endif
        initializeI2CBus();
        return true;
    }
    return false;
}
