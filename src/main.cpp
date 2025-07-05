#include <Arduino.h>

#include <lvgl.h>
#include "lgfx_config.h"
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <time.h>
#include <ArduinoJson.h>
#include <ui.h>
#include "UIManager.h"
#include "ConfigManager.h"
#include "debug_config.h"
#include <screens.h>  // Include for the objects struct from EEZ Studio UI
#include "HardwareConfig.h"  // Include for hardware configuration constants
#include <ArduinoOTA.h>
#include "OtaManager.h"
#include "AlarmManager.h"
#include "EventHandler.h"

// Forward declarations
void my_log_cb(lv_log_level_t level, const char *buf);
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
void my_touchpad_read(lv_indev_t *drv, lv_indev_data_t *data);
void listDirectory(fs::FS &fs, const char *dirname, uint8_t levels);
void connectToWiFi();
void initializeWeatherService();
void syncTimeFromNTP();
void updateWiFiStatusUI();
void startPeriodicTasks();
void wifiStatusTimerCallback(lv_timer_t *timer);
void timeUpdateTimerCallback(lv_timer_t *timer);
void envSensorTimerCallback(lv_timer_t *timer);
void weatherUpdateTimerCallback(lv_timer_t *timer);



// LVGL display and input device
lv_display_t * display = NULL;
lv_indev_t * touch_indev = NULL;

// LVGL timers
lv_timer_t * wifi_status_timer = NULL;
lv_timer_t * time_update_timer = NULL;
lv_timer_t * env_sensor_timer = NULL;
lv_timer_t * weather_update_timer = NULL;

// Variable to store last WiFi status update time
unsigned long lastWiFiUpdateTime = 0;

// UIManager instance
UIManager* uiManager = nullptr;

// OtaManager instance
OtaManager otaManager;

#include <ui.h>
#include "HardwareConfig.h"

// Forward declare SD card filesystem driver functions for LVGL
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p);
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);

/*Don't forget to set Sketchbook location in File/Preferences to the path of your UI project (the parent folder of this INO file)*/

/*Change to your screen resolution*/
static const uint16_t screenWidth  = 800;
static const uint16_t screenHeight = 480;

// LovyanGFX instance
static LGFX gfx;

#if LV_USE_LOG != 0
/* LVGL 9.2.2+ logging callback */
void my_log_cb(lv_log_level_t level, const char *buf)
{
    (void)level;  // Unused parameter
    if (Serial) {
        Serial.print("[LVGL] ");
        Serial.print(buf);
        if (buf[strlen(buf)-1] != '\n') {
            Serial.println();
        }
        Serial.flush();
    }
}
#endif

/* Display flushing using low-level LovyanGFX commands for robustness */
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;

    gfx.startWrite();
    gfx.setAddrWindow(area->x1, area->y1, w, h);
    gfx.writePixels(reinterpret_cast<uint16_t*>(px_map), w * h);
    gfx.endWrite();

    lv_display_flush_ready(disp);
}

/* Read the touchpad using LovyanGFX integrated touch */
void my_touchpad_read(lv_indev_t *drv, lv_indev_data_t *data)
{
    uint16_t touchX, touchY;
    bool touched = gfx.getTouch(&touchX, &touchY);
    
    if (touched) {
        data->state = LV_INDEV_STATE_PRESSED;
        data->point.x = touchX;
        data->point.y = touchY;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

// Custom tick callback function for LVGL 9.x compatibility
uint32_t my_tick_get_cb(void) {
    return millis();
}



void setup()
{
    Serial.begin(115200);
    while(!Serial && millis() < 3000); // Wait up to 3 seconds for Serial
    #if SYSTEM_DEBUG
    delay(5000);
    #endif
    if (Serial) {
        Serial.println("=====================================");
        Serial.println("   RadioWecker SLS AI - Starting    ");
        Serial.println("=====================================");

#if SYSTEM_DEBUG
    if (psramFound()) {
        Serial.printf("PSRAM found and initialized. Total size: %u bytes, Free: %u bytes\n", ESP.getPsramSize(), ESP.getFreePsram());
    } else {
        Serial.println("No PSRAM found or PSRAM failed to initialize.");
    }
#endif
        Serial.flush();
    }

    // Initialize display
    if (Serial) Serial.println("Initializing Display...");
    if (!gfx.init())
    {
        if (Serial) {
            Serial.println("ERROR: gfx.init() failed!");
        }
    }
    else
    {
        if (Serial) {
            Serial.printf("Display initialized: %dx%d\n", gfx.width(), gfx.height());
        }
        
        // LVGL will handle byte swapping via LV_COLOR_16_SWAP, so no need for LovyanGFX to do it.

        // Clear screen before starting LVGL
        gfx.fillScreen(0x0000);
    }
    
    // Print LVGL version
    if (Serial) {
        Serial.printf("LVGL Version: %d.%d.%d\n", LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH);
    }
    
    Serial.println("Initializing LVGL...");
    lv_init();
    // Initialize LVGL tick callback with proper signature for LVGL 9.x
    lv_tick_set_cb(my_tick_get_cb);

    Serial.println("LVGL initialized");

    // Create a display driver
    display = lv_display_create(screenWidth, screenHeight);
    lv_display_set_flush_cb(display, my_disp_flush);

    // Use PARTIAL_MODE with two buffers in PSRAM.
    // This is a robust and flicker-free approach when driver-level double buffering is not available or configured.
    if (Serial) Serial.println("Allocating LVGL draw buffers in PSRAM for PARTIAL_MODE.");
    uint32_t buf_size = screenWidth * 100 * sizeof(lv_color_t); // 100 lines buffer
    void *ps_buf1 = ps_malloc(buf_size);
    void *ps_buf2 = ps_malloc(buf_size);

    if (ps_buf1 && ps_buf2) {
        lv_display_set_buffers(display, ps_buf1, ps_buf2, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);
        if (Serial) Serial.println("LVGL configured for PARTIAL_MODE with PSRAM buffers.");
    } else {
        if (Serial) Serial.println("PSRAM allocation failed. Using smaller internal RAM buffers as a fallback.");
        // Fallback to smaller internal RAM buffers if PSRAM allocation fails
        static lv_color_t int_buf1[screenWidth * 10];
        static lv_color_t int_buf2[screenWidth * 10];
        lv_display_set_buffers(display, int_buf1, int_buf2, sizeof(int_buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);
    }

    // Initialize touch
    touch_indev = lv_indev_create();
    if (!touch_indev) {
        if (Serial) {
            Serial.println("ERROR: Failed to create LVGL input device!");
        }
    } else {
        lv_indev_set_type(touch_indev, LV_INDEV_TYPE_POINTER);
        lv_indev_set_read_cb(touch_indev, my_touchpad_read);
        
        // Set the display for the input device
        if (display) {
            lv_indev_set_display(touch_indev, display);
        } else {
            Serial.println("WARNING: Display not available for touch input device");
        }
        
        // Enable the input device
        lv_indev_enable(touch_indev, true);
    }

    // Initialize SD Card
    Serial.println("Initializing SD card...");
    SPI.begin(SD_SCK, SD_MISO, SD_MOSI);
    if (!SD.begin(SD_CS)) {
        Serial.println("SD Card initialization failed!");
    } else {
        Serial.println("SD Card initialization successful!");
        
        // Initialize LVGL filesystem driver for SD card
        static lv_fs_drv_t fs_drv;
        lv_fs_drv_init(&fs_drv);

        // Set up driver letter and callbacks
        fs_drv.letter = DRIVE_LETTER;
        fs_drv.cache_size = 0;
        
        fs_drv.open_cb = fs_open;
        fs_drv.close_cb = fs_close;
        fs_drv.read_cb = fs_read;
        fs_drv.seek_cb = fs_seek;
        fs_drv.tell_cb = fs_tell;

        // Register the driver
        lv_fs_drv_register(&fs_drv);
        Serial.printf("LVGL filesystem driver registered with letter '%c:'", DRIVE_LETTER);
        Serial.println();
        
#if SD_DEBUG
        // List files on SD card
        Serial.println("Files on SD Card:");
        listDirectory(SD, "/", 0);
#endif
    }

    // Initialize I2C bus for sensors once, before UIManager initialization
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, I2C_FREQUENCY);  // Using constants from HardwareConfig.h
    
    // OTA is initialized in connectToWiFi() after a successful connection

    ui_init();

    // Event handlers for screen load/unload are now assigned in EEZ-Flow Studio.
    // Removing the manual registration here to prevent double execution.
    // Event handlers for save, cancel, add, and edit buttons are now assigned in EEZ-Flow Studio.
    // Removing the manual registration here to prevent double execution.
    lv_obj_add_event_cb(objects.delete_alarm_button, delete_button_event_handler, LV_EVENT_CLICKED, NULL);

    // Initialize AlarmManager. The constructor now handles loading alarms.
    AlarmManager* am = AlarmManager::getInstance();

    // Now that alarms are loaded (or not), populate the UI list
    am->populateAlarmList();

    // All alarm-related event handlers are now managed by EEZ-Flow User Actions for a consistent architecture.

    
    // Initialize UIManager and sensors
    uiManager = UIManager::getInstance();
    if (uiManager->initSensors()) {
#if SENSOR_DEBUG
        Serial.println("Environmental sensors initialized successfully");
#endif
    } else {
#if SENSOR_DEBUG
        Serial.println("Failed to initialize one or more environmental sensors");
#endif
    }
    Serial.println("UI initialized");
    
    // Now that UI is initialized, connect to WiFi using credentials from config.json
    connectToWiFi();
    
    // Start periodic tasks (WiFi status updates, etc.)
    startPeriodicTasks();

    Serial.println("Setup done");
}



void connectToWiFi() {
#if WIFI_DEBUG
    Serial.println("Reading WiFi credentials from config...");
#endif
    
    ConfigManager* configManager = ConfigManager::getInstance();
    
    // Initialize the configuration manager
    if (!configManager->isConfigLoaded()) {
        if (!configManager->initConfig()) {
#if CONFIG_DEBUG
            Serial.println("Error: Failed to initialize configuration manager");
#endif
            return;
        }
    }
    
    // Get WiFi credentials from config
    String wifiSSID, wifiPassword;
    if (!configManager->getWiFiCredentials(wifiSSID, wifiPassword)) {
#if CONFIG_DEBUG
        Serial.println("Error: WiFi credentials not found or incomplete!");
#endif
        return;
    }
    
    // Connect to WiFi
#if WIFI_DEBUG
    Serial.printf("Connecting to WiFi SSID: %s\n", wifiSSID.c_str());
#endif
    
    WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());
    
    int timeout = 0;
    while (WiFi.status() != WL_CONNECTED && timeout < 20) {
        delay(500);
#if WIFI_DEBUG
        Serial.print(".");
#endif
        timeout++;
    }
    
    if (WiFi.status() != WL_CONNECTED) {
#if WIFI_DEBUG
        Serial.println("\nFailed to connect to WiFi!");
#endif
        return;
    }
    
#if WIFI_DEBUG
    Serial.println("\nWiFi connected! IP address: " + WiFi.localIP().toString());
#endif
    
    // After connecting to WiFi, initialize OTA
    otaManager.begin();
    syncTimeFromNTP();
    // Check if UIManager is initialized before initializing weather
    if (!uiManager) {
#if CONFIG_DEBUG
        Serial.println("Error: UIManager not initialized!");
#endif
        return;
    }
    
    // Initialize weather service with configuration
    initializeWeatherService();
}

void initializeWeatherService() {
    ConfigManager* configManager = ConfigManager::getInstance();
    
    // Extract weather configuration
    String apiKey;
    float latitude, longitude;
    String units, language;
    
    if (!configManager->getWeatherSettings(apiKey, latitude, longitude, units, language)) {
#if WEATHER_DEBUG
        Serial.println("No weather configuration found or configuration is incomplete");
#endif
        return;
    }
    
#if WEATHER_DEBUG
    Serial.println("Weather configuration found, initializing service...");
    Serial.printf("API Key: [%s]\n", apiKey.length() > 0 ? apiKey.c_str() : "EMPTY");
    Serial.printf("Location: [%.6f, %.6f]\n", latitude, longitude);
    Serial.printf("Units: [%s], Language: [%s]\n", units.c_str(), language.c_str());
#endif
    
    // Initialize weather service
    if (uiManager->initWeatherService(apiKey, latitude, longitude, units, language)) {
#if WEATHER_DEBUG
        Serial.println("WeatherService initialized successfully");
#endif
    } else {
#if WEATHER_DEBUG
        Serial.println("Failed to initialize WeatherService");
#endif
    }
}

// NTP time synchronization function
void syncTimeFromNTP() {
#if TIME_DEBUG
    Serial.println("Synchronizing time from NTP...");
#endif
    
    ConfigManager* configManager = ConfigManager::getInstance();
    
    // Get NTP settings from config
    String ntpServer, timezone;
    configManager->getNTPSettings(ntpServer, timezone);
    
#if TIME_DEBUG
    Serial.print("NTP Server: ");
    Serial.println(ntpServer);
    Serial.print("Timezone: ");
    Serial.println(timezone);
#endif
    
    // Configure time with NTP server and timezone
    configTzTime(timezone.c_str(), ntpServer.c_str(), "time.nist.gov");
    
    // Wait for time to be set
    struct tm timeinfo;
    int retry = 0;
    const int maxRetries = 10;
    
#if TIME_DEBUG
    Serial.println("Waiting for NTP synchronization");
#endif

    while (!getLocalTime(&timeinfo) && retry < maxRetries) {
#if TIME_DEBUG
        Serial.print(".");
#endif
        delay(1000);
        retry++;
    }
    
    if (retry < maxRetries) {
#if TIME_DEBUG
        Serial.println();
        Serial.println("Time synchronized!");
        
        // Display the current time
        char timeStr[64];
        strftime(timeStr, sizeof(timeStr), "%A, %B %d %Y %H:%M:%S", &timeinfo);
        Serial.print("Current time: ");
        Serial.println(timeStr);
#endif
    } else {
#if TIME_DEBUG
        Serial.println();
        Serial.println("Failed to synchronize time!");
#endif
    }
}

// LVGL Filesystem Driver Functions

static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode) {
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;
    void * f = NULL;
    
    if(mode == LV_FS_MODE_RD) {
        // Remove drive letter from path (e.g. S:/foo.txt -> /foo.txt)
        if(path[0] == DRIVE_LETTER && path[1] == ':') {
            path += 2;
        }
        
        // Open the file for reading
        File file = SD.open(path);
        if(file) {
            // Allocate memory to store File object pointer
            f = malloc(sizeof(File));
            if(f) {
                memcpy(f, &file, sizeof(File));
                res = LV_FS_RES_OK;
            } else {
                file.close();
            }
        }
    }
    
    if(res != LV_FS_RES_OK) {
        return NULL;
    }
    return f;
}

static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p) {
    if(file_p) {
        File * fp = (File*)file_p;
        fp->close();
        free(file_p);
    }
    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br) {
    if(file_p == NULL) return LV_FS_RES_INV_PARAM;
    
    File * fp = (File*)file_p;
    *br = fp->read((uint8_t*)buf, btr);
    return (*br <= 0) ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence) {
    if(file_p == NULL) return LV_FS_RES_INV_PARAM;
    
    File * fp = (File*)file_p;
    
    SeekMode mode;
    if(whence == LV_FS_SEEK_SET) {
        mode = SeekSet;
    } else if(whence == LV_FS_SEEK_CUR) {
        mode = SeekCur;
    } else if(whence == LV_FS_SEEK_END) {
        mode = SeekEnd;
    } else {
        return LV_FS_RES_INV_PARAM;
    }
    
    if(fp->seek(pos, mode)) {
        return LV_FS_RES_OK;
    } else {
        return LV_FS_RES_UNKNOWN;
    }
}

static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p) {
    if(file_p == NULL) return LV_FS_RES_INV_PARAM;
    
    File * fp = (File*)file_p;
    *pos_p = fp->position();
    return LV_FS_RES_OK;
}

// WiFi status update is now handled by UIManager

// WiFi status update timer callback
void wifiStatusTimerCallback(lv_timer_t * timer) {
    if (uiManager) {
        uiManager->updateWiFiStatusUI();
    }
}

// Environmental sensor update timer callback
void envSensorTimerCallback(lv_timer_t * timer) {
    if (uiManager) {
        uiManager->updateEnvironmentalData();
    }
}

// Time and date update functions have been moved to UIManager class

// Callback for time update timer
void timeUpdateTimerCallback(lv_timer_t *timer) {
    // Update time using UIManager
    if (uiManager) {
        uiManager->updateTimeUI();
        
        // Check if hour has changed to update date as well
        struct tm timeinfo;
        if (getLocalTime(&timeinfo)) {
            if (timeinfo.tm_hour != uiManager->getLastUpdateHour()) {
                uiManager->updateDateUI();
            }
        }
    }
}

// Callback for weather update timer
void weatherUpdateTimerCallback(lv_timer_t *timer) {
    if (uiManager) {
        uiManager->updateWeatherData();
    }
}

// Start all periodic tasks
void startPeriodicTasks() {
    // Call the UIManager's updateWiFiStatusUI method to properly initialize the labels
    // UIManager now handles applying the appropriate color to the WiFi quality label
    if (uiManager) {
        uiManager->updateWiFiStatusUI();
    }
    
    // Create a timer that runs every 10 seconds (10000ms) for WiFi status updates
    wifi_status_timer = lv_timer_create(wifiStatusTimerCallback, 10000, NULL);
    
    // Create a timer that runs every second (1000ms) for time updates
    time_update_timer = lv_timer_create(timeUpdateTimerCallback, 1000, NULL);
    
    // Create a timer that runs every 30 seconds (30000ms) for environmental sensor updates
    env_sensor_timer = lv_timer_create(envSensorTimerCallback, 30000, NULL);
    
    // Create a timer that runs every 5 minutes (300000ms) for weather data updates
    weather_update_timer = lv_timer_create(weatherUpdateTimerCallback, 300000, NULL);
    
    // Run the first updates immediately using UIManager
    if (uiManager) {
        uiManager->updateWiFiStatusUI();
        uiManager->updateTimeUI();
        uiManager->updateDateUI();
    }
    
    // Immediately run environment sensor update
    if (uiManager) {
        uiManager->updateEnvironmentalData();
    }
    
    // Initial weather update
    if (uiManager) {
        uiManager->updateWeatherData();
    }
}



void loop()
{
    static uint32_t last_print = 0;
    static uint32_t last_touch_check = 0;
    static bool first_run = true;
    static uint32_t last_tick = 0;
    uint32_t now = millis();
    
    // Update LVGL tick counter - required for proper timing
    if(now - last_tick > 0) {
        lv_tick_inc(now - last_tick);
        last_tick = now;
    }
    
    // Handle LVGL tasks
    ArduinoOTA.handle();
    lv_timer_handler();
    ui_tick();
    
    // Force reinitialize touch if needed (only on first run)
    if (first_run) {
        // If touch is not initialized properly after 3 seconds, try to reinitialize
        if (now > 3000) {
#if TOUCH_DEBUG
            Serial.println("\n===== TOUCH AVAILABLE THROUGH LOVYANGFX =====\n");
#endif
        }
        // After 5 seconds, consider first run complete
        if (now > 5000) {
            first_run = false;
        }
    }
    
    // Periodically print debug info
    if (now - last_print > 1000) {  // Every second
        if (Serial) {
#if SYSTEM_DEBUG
            // System debug info
            Serial.printf("Free heap: %d bytes\n", (int)ESP.getFreeHeap());
#endif
            
#if TOUCH_DEBUG
            // Only print touch debug info if TOUCH_DEBUG is enabled
            Serial.println("Touch device is available through LovyanGFX");
            // Try to read touch to keep it active
            uint16_t touchX, touchY;
            bool touched = gfx.getTouch(&touchX, &touchY);
            if (touched) {
                Serial.printf("Touch active at X: %d, Y: %d\n", touchX, touchY);
            }
#else
            // If not in debug mode, just silently keep touch active
            uint16_t touchX, touchY;
            gfx.getTouch(&touchX, &touchY);
#endif
        }
        last_print = now;
    }
    
    // Check for touch input
    if (now - last_touch_check > 100) {  // Every 100ms
        if (touch_indev) {
            // Force a read of the touch device
            lv_indev_get_read_timer(touch_indev);
        }
        last_touch_check = now;
    }
    
    // Small delay to prevent watchdog reset
    delay(2);
}
