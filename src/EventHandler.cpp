#include "EventHandler.h"
#include "AlarmManager.h"
#include <ui.h> // For objects struct
#include "debug_config.h"
#include <Arduino.h>
#include <screens.h> // For objects struct

// Event handler for when the alarm edit screen is loaded
void alarm_edit_screen_load_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SCREEN_LOADED) {
        AlarmManager* am = AlarmManager::getInstance();

        const Alarm* alarm = am->getEditingAlarm();

        if (alarm) { // Editing existing alarm
            #if ALARM_DEBUG
            Serial.printf("Loading alarm %d into edit screen.\n", alarm->id);
            #endif
            // Populate UI fields
            lv_textarea_set_text(objects.alarm_title_textarea, alarm->title.c_str());
            lv_roller_set_selected(objects.hour_roller, alarm->hour, LV_ANIM_OFF);
            lv_roller_set_selected(objects.minute_roller, alarm->minute, LV_ANIM_OFF);
            
            // Set repeat and weekdays
            if(alarm->repeat) lv_obj_add_state(objects.alarm_repeat_switch, LV_STATE_CHECKED);
            else lv_obj_clear_state(objects.alarm_repeat_switch, LV_STATE_CHECKED);

            if(alarm->weekdays[0]) lv_obj_add_state(objects.monday_checkbox, LV_STATE_CHECKED); else lv_obj_clear_state(objects.monday_checkbox, LV_STATE_CHECKED);
            if(alarm->weekdays[1]) lv_obj_add_state(objects.tuesday_checkbox, LV_STATE_CHECKED); else lv_obj_clear_state(objects.tuesday_checkbox, LV_STATE_CHECKED);
            if(alarm->weekdays[2]) lv_obj_add_state(objects.wednesday_checkbox, LV_STATE_CHECKED); else lv_obj_clear_state(objects.wednesday_checkbox, LV_STATE_CHECKED);
            if(alarm->weekdays[3]) lv_obj_add_state(objects.thursday_checkbox, LV_STATE_CHECKED); else lv_obj_clear_state(objects.thursday_checkbox, LV_STATE_CHECKED);
            if(alarm->weekdays[4]) lv_obj_add_state(objects.friday_checkbox, LV_STATE_CHECKED); else lv_obj_clear_state(objects.friday_checkbox, LV_STATE_CHECKED);
            if(alarm->weekdays[5]) lv_obj_add_state(objects.saturday_checkbox, LV_STATE_CHECKED); else lv_obj_clear_state(objects.saturday_checkbox, LV_STATE_CHECKED);
            if(alarm->weekdays[6]) lv_obj_add_state(objects.sunday_checkbox, LV_STATE_CHECKED); else lv_obj_clear_state(objects.sunday_checkbox, LV_STATE_CHECKED);

            lv_label_set_text(objects.alarm_date_label, alarm->date.c_str());

        } else { // Adding new alarm
            #if ALARM_DEBUG
            Serial.println("Setting default values for new alarm screen.");
            #endif
            // Set to default values
            lv_textarea_set_text(objects.alarm_title_textarea, "New Alarm");
            lv_roller_set_selected(objects.hour_roller, 7, LV_ANIM_OFF);
            lv_roller_set_selected(objects.minute_roller, 0, LV_ANIM_OFF);
            lv_obj_clear_state(objects.alarm_repeat_switch, LV_STATE_CHECKED);
            lv_obj_clear_state(objects.monday_checkbox, LV_STATE_CHECKED);
            lv_obj_clear_state(objects.tuesday_checkbox, LV_STATE_CHECKED);
            lv_obj_clear_state(objects.wednesday_checkbox, LV_STATE_CHECKED);
            lv_obj_clear_state(objects.thursday_checkbox, LV_STATE_CHECKED);
            lv_obj_clear_state(objects.friday_checkbox, LV_STATE_CHECKED);
            lv_obj_clear_state(objects.saturday_checkbox, LV_STATE_CHECKED);
            lv_obj_clear_state(objects.sunday_checkbox, LV_STATE_CHECKED);
            lv_label_set_text(objects.alarm_date_label, ""); // No date for new alarm yet
        }
    }
}

// Event handler for the "Save" button on the alarm edit screen
void save_button_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        #if ALARM_DEBUG
        Serial.println("Save button clicked. Saving data...");
        #endif
        AlarmManager* am = AlarmManager::getInstance();
        Alarm alarm_data;

        // Collect data from UI widgets
        if (am->getEditMode() == AlarmEditMode::EDIT && am->getEditingAlarm() != nullptr) {
            alarm_data.id = am->getEditingAlarm()->id;
        } else {
            alarm_data.id = am->getNextAlarmId();
        }

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
        #if ALARM_DEBUG
        Serial.println("Add Alarm button clicked.");
        #endif
        AlarmManager::getInstance()->startAddAlarm();
        // Screen transition is now handled by EEZ-Flow
    }
}

// Event handler for the "Cancel" button on the alarm edit screen
void cancel_button_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        #if ALARM_DEBUG
        Serial.println("Cancel button clicked. No action needed.");
        #endif
        // Screen transition is now handled by EEZ-Flow
    }
}

// Event handler for clicking on an alarm entry
void alarm_entry_click_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        lv_obj_t* target_obj = static_cast<lv_obj_t*>(lv_event_get_target(e));
                // The user data is the alarm ID, cast from a pointer.
        intptr_t alarm_id_ptr = reinterpret_cast<intptr_t>(lv_event_get_user_data(e));
        int alarm_id = static_cast<int>(alarm_id_ptr);
        AlarmManager::getInstance()->setSelectedAlarm(alarm_id, target_obj);
    }
}

// Event handler for the main "Edit" button
void edit_button_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        AlarmManager* am = AlarmManager::getInstance();
        int selectedId = am->getSelectedAlarmId();
        if (selectedId != -1) {
            #if ALARM_DEBUG
            Serial.printf("Edit button clicked for alarm %d.\n", selectedId);
            #endif
            am->startEditAlarm(selectedId);
            // Screen transition is handled by EEZ-Flow
        }
    }
}

// Event handler for the main "Delete" button
void delete_button_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        #if ALARM_DEBUG
        Serial.println("Delete button clicked.");
        #endif
        AlarmManager::getInstance()->deleteSelectedAlarm();
    }
}
