#include "AlarmManager.h"
#include "EventHandler.h"
#include "actions.h" // For objects struct
#include "debug_config.h"
#include <Arduino.h>
#include <screens.h> // For objects struct

static lv_obj_t *keyboard = nullptr;

// Event handler for the alarm title text area to manage the keyboard
void alarm_title_textarea_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
        lv_obj_t *ta = static_cast<lv_obj_t*>(lv_event_get_target(e));
    
    if (code == LV_EVENT_FOCUSED) {
        if (keyboard) {
            lv_keyboard_set_textarea(keyboard, ta);
            lv_obj_clear_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
        }
    } else if (code == LV_EVENT_DEFOCUSED) {
        if (keyboard) {
            lv_keyboard_set_textarea(keyboard, NULL);
            lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
        }
    } else if (code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
        if (keyboard) {
            lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
        }
        lv_obj_clear_state(ta, LV_STATE_FOCUSED);
        lv_indev_reset(NULL, ta); // To remove the cursor
    }
}


// Populates the alarm edit screen with data from the AlarmManager
static void populate_alarm_edit_screen() {
    AlarmManager* am = AlarmManager::getInstance();
    const Alarm* alarm = am->getEditingAlarm();

    if (alarm) { // Editing existing alarm
        #if ALARM_UI_DEBUG
        Serial.printf("Populating edit screen for alarm %d.\n", alarm->id);
        #endif
        // Populate UI fields
        lv_textarea_set_text(objects.alarm_title_textarea, alarm->title.c_str());
        lv_roller_set_selected(objects.hour_roller, alarm->hour, LV_ANIM_OFF);
        lv_roller_set_selected(objects.minute_roller, alarm->minute, LV_ANIM_OFF);
        
        if (alarm->repeat) {
            lv_obj_add_state(objects.alarm_repeat_switch, LV_STATE_CHECKED);
        } else {
            lv_obj_clear_state(objects.alarm_repeat_switch, LV_STATE_CHECKED);
        }

        // Manually trigger the VALUE_CHANGED event to ensure the UI (e.g., panels)
        // updates based on the switch's new state. This is necessary because
        // lv_obj_add/clear_state does not fire this event automatically.
        #if ALARM_UI_DEBUG
        Serial.println("Manually sending VALUE_CHANGED event to alarm_repeat_switch.");
        #endif
        lv_obj_send_event(objects.alarm_repeat_switch, LV_EVENT_VALUE_CHANGED, NULL);

        if(alarm->weekdays[0]) lv_obj_add_state(objects.monday_checkbox, LV_STATE_CHECKED); else lv_obj_clear_state(objects.monday_checkbox, LV_STATE_CHECKED);
        if(alarm->weekdays[1]) lv_obj_add_state(objects.tuesday_checkbox, LV_STATE_CHECKED); else lv_obj_clear_state(objects.tuesday_checkbox, LV_STATE_CHECKED);
        if(alarm->weekdays[2]) lv_obj_add_state(objects.wednesday_checkbox, LV_STATE_CHECKED); else lv_obj_clear_state(objects.wednesday_checkbox, LV_STATE_CHECKED);
        if(alarm->weekdays[3]) lv_obj_add_state(objects.thursday_checkbox, LV_STATE_CHECKED); else lv_obj_clear_state(objects.thursday_checkbox, LV_STATE_CHECKED);
        if(alarm->weekdays[4]) lv_obj_add_state(objects.friday_checkbox, LV_STATE_CHECKED); else lv_obj_clear_state(objects.friday_checkbox, LV_STATE_CHECKED);
        if(alarm->weekdays[5]) lv_obj_add_state(objects.saturday_checkbox, LV_STATE_CHECKED); else lv_obj_clear_state(objects.saturday_checkbox, LV_STATE_CHECKED);
        if(alarm->weekdays[6]) lv_obj_add_state(objects.sunday_checkbox, LV_STATE_CHECKED); else lv_obj_clear_state(objects.sunday_checkbox, LV_STATE_CHECKED);

        lv_label_set_text(objects.alarm_date_label, alarm->date.c_str());

    } else { // Adding new alarm
        #if ALARM_UI_DEBUG
        Serial.println("Populating edit screen with default values for new alarm.");
        #endif
        // Set to default values
        lv_textarea_set_text(objects.alarm_title_textarea, "New Alarm");
        lv_roller_set_selected(objects.hour_roller, 7, LV_ANIM_OFF);
        lv_roller_set_selected(objects.minute_roller, 0, LV_ANIM_OFF);
        lv_obj_add_state(objects.alarm_repeat_switch, LV_STATE_CHECKED); // Default to repeat on
        lv_obj_add_state(objects.monday_checkbox, LV_STATE_CHECKED);
        lv_obj_add_state(objects.tuesday_checkbox, LV_STATE_CHECKED);
        lv_obj_add_state(objects.wednesday_checkbox, LV_STATE_CHECKED);
        lv_obj_add_state(objects.thursday_checkbox, LV_STATE_CHECKED);
        lv_obj_add_state(objects.friday_checkbox, LV_STATE_CHECKED);
        lv_obj_clear_state(objects.saturday_checkbox, LV_STATE_CHECKED);
        lv_obj_clear_state(objects.sunday_checkbox, LV_STATE_CHECKED);
        lv_label_set_text(objects.alarm_date_label, "");

        // Manually trigger the VALUE_CHANGED event to ensure the UI (e.g., panels)
        // updates based on the switch's new state. This is necessary because
        // lv_obj_add/clear_state does not fire this event automatically.
        #if ALARM_UI_DEBUG
        Serial.println("Manually sending VALUE_CHANGED event to alarm_repeat_switch for new alarm.");
        #endif
        lv_obj_send_event(objects.alarm_repeat_switch, LV_EVENT_VALUE_CHANGED, NULL);
    }
}

// Event handler for when the alarm edit screen is loaded or unloaded
void alarm_edit_screen_load_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_SCREEN_LOAD_START) {
        #if ALARM_UI_DEBUG
        Serial.println("Alarm edit screen: LOAD_START. Creating keyboard.");
        #endif
        // Create and configure the keyboard for the text area
        keyboard = lv_keyboard_create(lv_screen_active());
        lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_event_cb(objects.alarm_title_textarea, alarm_title_textarea_event_handler, LV_EVENT_ALL, NULL);
    } else if (code == LV_EVENT_SCREEN_LOADED) {
        #if ALARM_UI_DEBUG
        Serial.println("Alarm edit screen: LOADED. Populating screen.");
        #endif
        populate_alarm_edit_screen();
    } else if (code == LV_EVENT_SCREEN_UNLOAD_START) {
        #if ALARM_UI_DEBUG
        Serial.println("Alarm edit screen: UNLOAD_START. Deleting keyboard.");
        #endif
        if (keyboard) {
            lv_obj_del(keyboard);
            keyboard = nullptr;
        }
        // It's good practice to remove the event callback to prevent it from being called on a deleted object
        lv_obj_remove_event_cb(objects.alarm_title_textarea, alarm_title_textarea_event_handler);
    }
}

// Event handler for the "Save" button on the alarm edit screen
void save_button_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        #if ALARM_UI_DEBUG
        Serial.println("Save button clicked. Saving data...");
        #endif
        AlarmManager* am = AlarmManager::getInstance();
        Alarm alarm_data;

        // Determine if we are adding a new alarm or editing an existing one
        if (am->getEditMode() == AlarmEditMode::EDIT) {
            // Editing an existing alarm
            const Alarm* current_alarm = am->getEditingAlarm();
            if (current_alarm) {
                alarm_data.id = current_alarm->id;
            } else {
                // Error state: in edit mode but no alarm is selected for editing.
                #if ALARM_UI_DEBUG
                Serial.println("ERROR: Save clicked in EDIT mode, but no alarm is being edited.");
                #endif
                return; // Abort save
            }
        } else {
            // Adding a new alarm
            alarm_data.id = am->getNextAlarmId();
        }

        // Collect data from UI widgets
        alarm_data.title = lv_textarea_get_text(objects.alarm_title_textarea);
        
        char buf[8];
        lv_roller_get_selected_str(objects.hour_roller, buf, sizeof(buf));
        alarm_data.hour = atoi(buf);
        lv_roller_get_selected_str(objects.minute_roller, buf, sizeof(buf));
        alarm_data.minute = atoi(buf);

        alarm_data.active = true; // By default, a new/edited alarm is active. This should be handled by the on/off switch.
        alarm_data.repeat = lv_obj_has_state(objects.alarm_repeat_switch, LV_STATE_CHECKED);

        alarm_data.weekdays[0] = lv_obj_has_state(objects.monday_checkbox, LV_STATE_CHECKED);
        alarm_data.weekdays[1] = lv_obj_has_state(objects.tuesday_checkbox, LV_STATE_CHECKED);
        alarm_data.weekdays[2] = lv_obj_has_state(objects.wednesday_checkbox, LV_STATE_CHECKED);
        alarm_data.weekdays[3] = lv_obj_has_state(objects.thursday_checkbox, LV_STATE_CHECKED);
        alarm_data.weekdays[4] = lv_obj_has_state(objects.friday_checkbox, LV_STATE_CHECKED);
        alarm_data.weekdays[5] = lv_obj_has_state(objects.saturday_checkbox, LV_STATE_CHECKED);
        alarm_data.weekdays[6] = lv_obj_has_state(objects.sunday_checkbox, LV_STATE_CHECKED);

        alarm_data.date = lv_label_get_text(objects.alarm_date_label);

        am->saveAlarm(alarm_data);
        am->populateAlarmList(); // Repopulate list to show changes

        // Screen transition is now handled by EEZ-Flow
    }
}

// Event handler for the "Add Alarm" button
void add_alarm_button_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        #if ALARM_UI_DEBUG
        Serial.println("Add Alarm button clicked. Setting state and populating screen.");
        #endif
        AlarmManager::getInstance()->deselectAlarm(); // Deselect to prevent stray focus events on the next screen
        AlarmManager::getInstance()->startAddAlarm();
        // The screen transition is initiated by EEZ-Flow.
        // The population will be handled by the LV_EVENT_SCREEN_LOADED event.
    }
}

// Event handler for the "Cancel" button on the alarm edit screen
void cancel_button_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        #if ALARM_UI_DEBUG
        Serial.println("Cancel button clicked. No action needed.");
        #endif
        // Screen transition is now handled by EEZ-Flow
    }
}

void alarm_entry_focus_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* target_obj = static_cast<lv_obj_t*>(lv_event_get_target(e));

    if (code == LV_EVENT_FOCUSED) {
        intptr_t alarm_id_ptr = reinterpret_cast<intptr_t>(lv_event_get_user_data(e));
        int alarm_id = static_cast<int>(alarm_id_ptr);
        AlarmManager::getInstance()->setSelectedAlarm(alarm_id, target_obj);

        #if ALARM_UI_DEBUG
        Serial.printf("Alarm %d focused.\n", alarm_id);
        #endif
    } else if (code == LV_EVENT_DEFOCUSED) {
        // Only deselect if an alarm is currently selected to prevent double-firing
        if (AlarmManager::getInstance()->getSelectedAlarmId() != -1) {
            AlarmManager::getInstance()->deselectAlarm();

            #if ALARM_UI_DEBUG
            Serial.println("Alarm defocused.");
            #endif
        }
    }
}

// Event handler for the main "Edit" button
void edit_button_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        AlarmManager* am = AlarmManager::getInstance();
        int alarmId = am->getSelectedAlarmId();
        if (alarmId != -1) {
            #if ALARM_UI_DEBUG
            Serial.printf("Edit button clicked for alarm %d. Setting state and populating screen.\n", alarmId);
            #endif
            am->deselectAlarm(); // Deselect to prevent stray focus events on the next screen
            am->startEditAlarm(alarmId);
            // The screen transition is initiated by EEZ-Flow.
            // The population will be handled by the LV_EVENT_SCREEN_LOADED event.
        } else {
            #if ALARM_UI_DEBUG
            Serial.println("Edit button clicked, but no alarm selected.");
            #endif
        }
    }
}

// Event handler for the main "Delete" button
void delete_button_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        #if ALARM_UI_DEBUG
        Serial.println("Delete button clicked.");
        #endif
        AlarmManager::getInstance()->deleteSelectedAlarm();
    }
}

// Static variables to store the original alarm date for cancel functionality
static char original_alarm_date_text[16] = {0};
static lv_calendar_date_t original_calendar_date = {0, 0, 0};
static bool has_original_date = false;

// Static variable to track the last selected date in calendar
static lv_calendar_date_t last_selected_date = {0, 0, 0};
static bool has_user_selected_date = false;

// Date selection event handler - called via EEZ Studio VALUE_CHANGED event
void calendar_date_selection_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_VALUE_CHANGED) {
        // Get the selected date from the calendar widget
        lv_calendar_get_pressed_date(objects.calendar_selector, &last_selected_date);
        has_user_selected_date = true;
        
        // Update the visual highlight on the calendar widget
        lv_calendar_set_highlighted_dates(objects.calendar_selector, &last_selected_date, 1);
        
        #if ALARM_UI_DEBUG
        Serial.printf("[CALENDAR] Date selected: %02d.%02d.%04d (highlighted)\n", 
                     last_selected_date.day, last_selected_date.month, last_selected_date.year);
        #endif
    }
}

// Helper function to parse date string and set calendar date
static void parse_date_string_to_calendar(const char* date_str, lv_calendar_date_t* date) {
    if (!date_str || strlen(date_str) == 0) {
        // No date set, use today's date
        time_t now = time(nullptr);
        struct tm* timeinfo = localtime(&now);
        date->year = timeinfo->tm_year + 1900;
        date->month = timeinfo->tm_mon + 1;
        date->day = timeinfo->tm_mday;
    } else {
        // Parse existing date (assuming format: YYYY-MM-DD or DD.MM.YYYY or similar)
        int day, month, year;
        if (sscanf(date_str, "%d.%d.%d", &day, &month, &year) == 3) {
            // DD.MM.YYYY format
            date->day = day;
            date->month = month;
            date->year = year;
        } else if (sscanf(date_str, "%d-%d-%d", &year, &month, &day) == 3) {
            // YYYY-MM-DD format
            date->day = day;
            date->month = month;
            date->year = year;
        } else {
            // Fallback to today's date if parsing fails
            time_t now = time(nullptr);
            struct tm* timeinfo = localtime(&now);
            date->year = timeinfo->tm_year + 1900;
            date->month = timeinfo->tm_mon + 1;
            date->day = timeinfo->tm_mday;
        }
    }
}

// Helper function to format calendar date to string
static void format_calendar_date_to_string(const lv_calendar_date_t* date, char* buffer, size_t buffer_size) {
    snprintf(buffer, buffer_size, "%02d.%02d.%04d", date->day, date->month, date->year);
}

// Event handler for the show calendar button
void show_calendar_button_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        #if ALARM_UI_DEBUG
        Serial.println("[CALENDAR] Show calendar button clicked. Initializing calendar.");
        #endif
        
        // Get the current alarm date from the label
        const char* current_date_str = lv_label_get_text(objects.alarm_date_label);
        
        // Parse the current date and set up the calendar
        lv_calendar_date_t date_to_show;
        parse_date_string_to_calendar(current_date_str, &date_to_show);
        
        // Store the original date for potential restoration on cancel
        original_calendar_date = date_to_show;
        has_original_date = (current_date_str && strlen(current_date_str) > 0);
        
        // Initialize date selection tracking
        last_selected_date = date_to_show;
        has_user_selected_date = false;
        
        // Set the calendar to show the correct month/year
        lv_calendar_set_showed_date(objects.calendar_selector, date_to_show.year, date_to_show.month);
        
        // Set today's date (required by LVGL calendar)
        time_t now = time(nullptr);
        struct tm* timeinfo = localtime(&now);
        lv_calendar_set_today_date(objects.calendar_selector, 
                                   timeinfo->tm_year + 1900, 
                                   timeinfo->tm_mon + 1, 
                                   timeinfo->tm_mday);
        
        // Highlight the current alarm date (or today if no date set)
        lv_calendar_set_highlighted_dates(objects.calendar_selector, &date_to_show, 1);
        
        #if ALARM_UI_DEBUG
        Serial.printf("Calendar initialized with date: %02d.%02d.%04d\n", 
                     date_to_show.day, date_to_show.month, date_to_show.year);
        #endif
        
        // The calendar popup will be shown by EEZ-Flow
    }
}

// Event handler for the calendar OK button
void calendar_ok_button_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        #if ALARM_UI_DEBUG
        Serial.println("[CALENDAR] Calendar OK button clicked.");
        #endif
        
        // Use the date tracked by our calendar event handler
        lv_calendar_date_t selected_date = last_selected_date;
        
        #if ALARM_UI_DEBUG
        Serial.printf("[CALENDAR] Selected date: %02d.%02d.%04d (user_selected: %s)\n", 
                     selected_date.day, selected_date.month, selected_date.year,
                     has_user_selected_date ? "yes" : "no");
        #endif
        
        // Only update the alarm date if a valid date was selected
        if (selected_date.year > 0 && selected_date.month > 0 && selected_date.day > 0) {
            char date_str[16];
            format_calendar_date_to_string(&selected_date, date_str, sizeof(date_str));
            
            // Update the alarm date label
            lv_label_set_text(objects.alarm_date_label, date_str);
            
            #if ALARM_UI_DEBUG
            Serial.printf("Calendar date selected and saved: %s\n", date_str);
            #endif
        } else {
            #if ALARM_UI_DEBUG
            Serial.println("No valid date selected in calendar, no changes made.");
            #endif
        }
        

        
        // The calendar popup will be hidden by EEZ-Flow
    }
}

// Event handler for the calendar Cancel button
void calendar_cancel_button_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        #if ALARM_UI_DEBUG
        Serial.println("[CALENDAR] Calendar Cancel button clicked. No changes made.");
        #endif
        
        // Restore the original date if there was one
        if (has_original_date) {
            lv_calendar_set_highlighted_dates(objects.calendar_selector, &original_calendar_date, 1);
        }
        

        
        // The calendar popup will be hidden by EEZ-Flow
        // No changes are made to the alarm_date_label
    }
}

//##############################################################################
// Radio Page Event Handlers
//##############################################################################

#include "AudioManager.h"
#include "ConfigManager.h"
#include "RadioData.h"

// External reference to global AudioManager instance
extern AudioManager audioManager;

/**
 * @brief Handler for radio station dropdown selection change
 * @param e LVGL event containing the dropdown selection
 */
void radio_station_changed_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_VALUE_CHANGED) {
#if AUDIO_DEBUG
        Serial.println("[RADIO] Station selection changed");
#endif
        // Note: Station data will be handled via EEZ Studio databinding
        // The selected station URL will be available through the StationList data source
        // No direct action needed here as the play button will use the selected station
    }
}

/**
 * @brief Handler for radio play button click
 * @param e LVGL event
 */
void radio_play_button_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
#if AUDIO_DEBUG
        Serial.println("[RADIO] Play button clicked");
#endif
        
        // Get the selected station from ConfigManager
        ConfigManager* configManager = ConfigManager::getInstance();
        if (configManager) {
            // Get radio stations array
            JsonArray stations = configManager->getRadioStations();
            if (!stations.isNull() && stations.size() > 0) {
                // For now, use the first station. In a full implementation,
                // this would get the selected station index from the UI dropdown
                // via EEZ Studio databinding
                JsonObject station = stations[0];
                String stationUrl = station["url"].as<String>();
                String stationName = station["name"].as<String>();
                
                if (stationUrl.length() > 0) {
#if AUDIO_DEBUG
                    Serial.printf("[RADIO] Starting playback: %s\n", stationName.c_str());
#endif
                    // Update PlaybackInfo struct for UI databinding
                    strncpy(g_playbackInfo.Title, stationName.c_str(), sizeof(g_playbackInfo.Title) - 1);
                    g_playbackInfo.Title[sizeof(g_playbackInfo.Title) - 1] = '\0';
                    strncpy(g_playbackInfo.Album, "Web Radio", sizeof(g_playbackInfo.Album) - 1);
                    g_playbackInfo.Album[sizeof(g_playbackInfo.Album) - 1] = '\0';
                    strncpy(g_playbackInfo.Artist, "", sizeof(g_playbackInfo.Artist) - 1);
                    g_playbackInfo.Artist[sizeof(g_playbackInfo.Artist) - 1] = '\0';
                    strncpy(g_playbackInfo.AlarmTitle, "", sizeof(g_playbackInfo.AlarmTitle) - 1); // Keep blank unless started from alarm
                    g_playbackInfo.AlarmTitle[sizeof(g_playbackInfo.AlarmTitle) - 1] = '\0';
                    
                    // Start audio playback
                    audioManager.connecttohost(stationUrl.c_str());
                } else {
#if AUDIO_DEBUG
                    Serial.println("[RADIO] Error: Station URL is empty");
#endif
                }
            } else {
#if AUDIO_DEBUG
                Serial.println("[RADIO] Error: No stations available");
#endif
            }
        }
    }
}

/**
 * @brief Handler for radio stop button click
 * @param e LVGL event
 */
void radio_stop_button_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
#if AUDIO_DEBUG
        Serial.println("[RADIO] Stop button clicked");
#endif
        
        // Stop audio playback
        audioManager.stop();
        
        // Clear PlaybackInfo struct
        strncpy(g_playbackInfo.Title, "", sizeof(g_playbackInfo.Title) - 1);
        g_playbackInfo.Title[sizeof(g_playbackInfo.Title) - 1] = '\0';
        strncpy(g_playbackInfo.Album, "", sizeof(g_playbackInfo.Album) - 1);
        g_playbackInfo.Album[sizeof(g_playbackInfo.Album) - 1] = '\0';
        strncpy(g_playbackInfo.Artist, "", sizeof(g_playbackInfo.Artist) - 1);
        g_playbackInfo.Artist[sizeof(g_playbackInfo.Artist) - 1] = '\0';
        strncpy(g_playbackInfo.AlarmTitle, "", sizeof(g_playbackInfo.AlarmTitle) - 1);
        g_playbackInfo.AlarmTitle[sizeof(g_playbackInfo.AlarmTitle) - 1] = '\0';
    }
}

/**
 * @brief Handler for radio volume slider change
 * @param e LVGL event containing the slider value
 */
void radio_volume_changed_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t* slider = static_cast<lv_obj_t*>(lv_event_get_target(e));
        int32_t volume = lv_slider_get_value(slider);
        
#if AUDIO_DEBUG
        Serial.printf("[RADIO] Volume changed to: %d%%\n", (int)volume);
#endif
        
        // Set audio volume (0-100 range)
        audioManager.setVolume(volume);
        
        // Save volume to config
        ConfigManager* configManager = ConfigManager::getInstance();
        if (configManager) {
            configManager->setRadioVolume((uint8_t)volume);
            configManager->saveConfig();
        }
    }
}
