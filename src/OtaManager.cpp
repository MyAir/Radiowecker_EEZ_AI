#include "OtaManager.h"
#include <ArduinoOTA.h>
#include <WiFi.h>

OtaManager::OtaManager() : ota_screen(NULL), ota_label(NULL), ota_progress_bar(NULL) {}

void OtaManager::begin() {
    ArduinoOTA.setHostname("RadioWeckerAI");
    ArduinoOTA.setPassword("admin");

    ArduinoOTA.onStart([this]() {
        if (ota_screen == NULL) {
            create_ota_screen();
        }
        lv_screen_load(ota_screen);
        lv_bar_set_value(ota_progress_bar, 0, LV_ANIM_OFF);
        lv_obj_set_style_bg_color(ota_progress_bar, lv_palette_main(LV_PALETTE_BLUE), LV_PART_INDICATOR);
        lv_label_set_text(ota_label, "Connecting...");
        lv_timer_handler();
        delay(10);
    });

    ArduinoOTA.onEnd([this]() {
        lv_label_set_text(ota_label, "Update Complete!\nRebooting...");
        lv_timer_handler();
        delay(10);
    });

    ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total) {
        static uint8_t last_percentage = 0;
        uint8_t percentage = (progress / (total / 100));
        if (percentage != last_percentage) {
            last_percentage = percentage;
            lv_bar_set_value(ota_progress_bar, percentage, LV_ANIM_ON);
            lv_label_set_text_fmt(ota_label, "Updating... %d%%", percentage);
            lv_timer_handler();
        }
    });

    ArduinoOTA.onError([this](ota_error_t error) {
        const char* error_msg;
        switch(error) {
            case OTA_AUTH_ERROR: error_msg = "Authentication Failed"; break;
            case OTA_BEGIN_ERROR: error_msg = "Begin Failed"; break;
            case OTA_CONNECT_ERROR: error_msg = "Connect Failed"; break;
            case OTA_RECEIVE_ERROR: error_msg = "Receive Failed"; break;
            case OTA_END_ERROR: error_msg = "End Failed"; break;
            default: error_msg = "Unknown Error"; break;
        }
        lv_label_set_text_fmt(ota_label, "ERROR: %s", error_msg);
        lv_obj_set_style_bg_color(ota_progress_bar, lv_palette_main(LV_PALETTE_RED), LV_PART_INDICATOR);
        lv_bar_set_value(ota_progress_bar, 100, LV_ANIM_OFF);
        lv_timer_handler();
    });

    ArduinoOTA.begin();

    if (Serial) {
        Serial.println("OTA Initialized");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    }
}

void OtaManager::create_ota_screen() {
    if (ota_screen) return;

    ota_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(ota_screen, lv_color_hex(0x000000), 0);

    lv_obj_t * title_label = lv_label_create(ota_screen);
    lv_label_set_text(title_label, "Firmware Update");
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(title_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 80);

    ota_progress_bar = lv_bar_create(ota_screen);
    lv_obj_set_size(ota_progress_bar, 400, 25);
    lv_obj_align(ota_progress_bar, LV_ALIGN_CENTER, 0, 0);
    lv_bar_set_value(ota_progress_bar, 0, LV_ANIM_OFF);

    ota_label = lv_label_create(ota_screen);
    lv_label_set_text(ota_label, "Waiting for update...");
    lv_obj_set_style_text_font(ota_label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(ota_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(ota_label, LV_ALIGN_BOTTOM_MID, 0, -100);
}
