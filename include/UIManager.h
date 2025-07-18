#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <Arduino.h>
#include <lvgl.h>
#include <Adafruit_SGP30.h>
#include <Adafruit_SHT31.h>
#include <Wire.h>
#include <WiFi.h>
#include "WeatherService.h"

// Baseline management for SGP30 sensor
#define SGP30_BASELINE_INTERVAL_MS 3600000  // Every hour (in milliseconds)

/**
 * @brief Singleton class for managing UI updates related to environmental sensors
 */
class UIManager {
private:
    // Singleton instance
    static UIManager* _instance;
    
    // Sensor objects
    Adafruit_SGP30* sgp30;
    Adafruit_SHT31* sht31;
    
    // Last readings for comparison to avoid unnecessary updates
    float lastTemperature = 0;
    float lastHumidity = 0;
    uint16_t lastTVOC = 0;
    uint16_t lastECO2 = 0;
    
    // Last WiFi status values
    String lastSSID = "";
    String lastIP = "";
    int lastRSSI = 0;
    bool lastConnected = false;
    
    // Sensor initialization status
    bool sgp30Initialized = false;
    bool sht31Initialized = false;
    
    // Flags to track first sensor readings
    bool firstTVOCReading = true;
    bool firstECO2Reading = true;
    
    // Time tracking for internal use
    int hourChangeTracking = -1;
    
    // Weather update tracking
    unsigned long lastWeatherUpdateTime = 0;
    
    // SGP30 baseline management
    unsigned long lastBaselineTime = 0;
    uint16_t eco2_baseline = 0;
    uint16_t tvoc_baseline = 0;
    
    // Private constructor for singleton pattern
    UIManager();
    
public:
    // Delete copy constructor and assignment operator
    UIManager(UIManager const&) = delete;
    void operator=(UIManager const&) = delete;
    
    // Get singleton instance
    static UIManager* getInstance();
    
    // Initialize sensors
    bool initSensors();
    
    // UI Update methods
    void updateTemperature(float temp);
    void updateHumidity(float humidity);
    void updateTVOC(uint16_t tvoc);
    void updateCO2(uint16_t eco2);
    void updateTimeUI();
    void updateDateUI();
    void updateWiFiStatusUI();
    
    // Read sensors and update UI
    void updateEnvironmentalData();
    
    // SGP30 baseline management
    void handleSGP30Baseline();
    
    // Sensor access methods
    Adafruit_SGP30* getSGP30() { return sgp30; }
    Adafruit_SHT31* getSHT31() { return sht31; }
    
    // Get initialization status
    bool isSGP30Initialized() { return sgp30Initialized; }
    bool isSHT31Initialized() { return sht31Initialized; }
    
    // Time tracking accessor
    int getLastUpdateHour() { return hourChangeTracking; }
    
    // WiFi status tracking
    bool hasWiFiStatusChanged();
    
    // Weather service methods
    bool initWeatherService(const String& apiKey, float latitude, float longitude, 
                          const String& units = "metric", const String& language = "de");
    void updateWeatherData();
    unsigned long getLastWeatherUpdateTime() { return lastWeatherUpdateTime; }
};

#endif // UIMANAGER_H
