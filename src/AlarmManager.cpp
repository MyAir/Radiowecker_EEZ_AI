#include "AlarmManager.h"
#include <ArduinoJson.h>
#include <SD.h>
#include <algorithm> // For std::remove_if
#include "debug_config.h"
#include "ui_helpers_extended.h"
#include "ui.h"
#include "HardwareConfig.h"
#include <screens.h> // For the 'objects' struct

// Initialize static instance pointer
AlarmManager* AlarmManager::m_instance = nullptr;

// Singleton instance getter
AlarmManager* AlarmManager::getInstance() {
    if (m_instance == nullptr) {
        m_instance = new AlarmManager();
    }
    return m_instance;
}

// Private constructor
AlarmManager::AlarmManager() : m_editMode(AlarmEditMode::NONE), m_editingAlarmId(-1), m_selectedAlarmId(-1), m_selectedAlarmObj(nullptr) {
    if (!SD.begin(SD_CS)) {
        DEBUG_PRINTLN("Card Mount Failed");
        return;
    }
    loadAlarms();
}

AlarmManager::~AlarmManager() {
    // Destructor
}

// Load alarms from JSON file on SD card
void AlarmManager::loadAlarms() {
    File file = SD.open("/alarms.json", FILE_READ);
    if (!file) {
#if ALARM_DEBUG
        DEBUG_PRINTLN("Error: Failed to open alarms.json for reading");
#endif
        return;
    }

    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
#if ALARM_DEBUG
        DEBUG_PRINT("Error: Failed to parse alarms.json: ");
        DEBUG_PRINTLN(error.c_str());
#endif
        return;
    }

    JsonArray array = doc.as<JsonArray>();
    if (array.isNull()) {
#if ALARM_DEBUG
        DEBUG_PRINTLN("Error: alarms.json is not a valid JSON array.");
#endif
        return;
    }

    m_alarms.clear();

    for (JsonObject obj : array) {
        Alarm alarm;
        alarm.id = obj["id"].as<int>();
        alarm.active = obj["active"].as<bool>();
        alarm.title = obj["title"].as<String>();
        alarm.hour = obj["hour"].as<int>();
        alarm.minute = obj["minute"].as<int>();
        alarm.repeat = obj["repeat"].as<bool>();
        alarm.date = obj["date"].as<String>();

        JsonArray weekdays = obj["weekdays"].as<JsonArray>();
        for (int i = 0; i < 7; ++i) {
            if (i < weekdays.size()) {
                alarm.weekdays[i] = weekdays[i].as<bool>();
            } else {
                alarm.weekdays[i] = false;
            }
        }
        m_alarms.push_back(alarm);
    }

#if ALARM_DEBUG
    DEBUG_PRINTF("Successfully loaded %d alarms from alarms.json\n", m_alarms.size());
#endif
}

const std::vector<Alarm>& AlarmManager::getAlarms() const {
    return m_alarms;
}

bool AlarmManager::saveAlarms() {
    File file = SD.open("/alarms.json", FILE_WRITE);
    if (!file) {
#if ALARM_DEBUG
        DEBUG_PRINTLN("Error: Failed to open alarms.json for writing");
#endif
        return false;
    }

    DynamicJsonDocument doc(2048);
    JsonArray array = doc.to<JsonArray>();

    for (const auto& alarm : m_alarms) {
        JsonObject obj = array.createNestedObject();
        obj["id"] = alarm.id;
        obj["active"] = alarm.active;
        obj["title"] = alarm.title;
        obj["hour"] = alarm.hour;
        obj["minute"] = alarm.minute;
        obj["repeat"] = alarm.repeat;
        obj["date"] = alarm.date;

        JsonArray weekdays = obj.createNestedArray("weekdays");
        for (int i = 0; i < 7; ++i) {
            weekdays.add(alarm.weekdays[i]);
        }
    }

    if (serializeJson(doc, file) == 0) {
#if ALARM_DEBUG
        DEBUG_PRINTLN("Error: Failed to write to alarms.json");
#endif
        file.close();
        return false;
    }

    file.close();
#if ALARM_DEBUG
    DEBUG_PRINTLN("Successfully saved alarms to alarms.json");
#endif
    return true;
}

void AlarmManager::startAddAlarm() {
    m_editMode = AlarmEditMode::ADD;
    m_editingAlarmId = -1;
}

void AlarmManager::startEditAlarm(int alarmId) {
    m_editMode = AlarmEditMode::EDIT;
    m_editingAlarmId = alarmId;
}

AlarmEditMode AlarmManager::getEditMode() const {
    return m_editMode;
}

Alarm* AlarmManager::getEditingAlarm() {
    if (m_editMode == AlarmEditMode::EDIT) {
        for (auto& alarm : m_alarms) {
            if (alarm.id == m_editingAlarmId) {
                return &alarm;
            }
        }
    }
    return nullptr;
}

void AlarmManager::saveAlarm(const Alarm& alarm) {
    bool found = false;
    for (auto& existing_alarm : m_alarms) {
        if (existing_alarm.id == alarm.id) {
            existing_alarm = alarm;
            found = true;
            break;
        }
    }
    if (!found) {
        m_alarms.push_back(alarm);
    }
    saveAlarms();
}

void AlarmManager::deleteAlarm(int alarmId) {
    auto it = std::remove_if(m_alarms.begin(), m_alarms.end(), [alarmId](const Alarm& alarm) {
        return alarm.id == alarmId;
    });
    if (it != m_alarms.end()) {
        m_alarms.erase(it, m_alarms.end());
        saveAlarms();
        #if ALARM_DEBUG
        DEBUG_PRINTF("Alarm with ID %d deleted.\n", alarmId);
        #endif
    } else {
        #if ALARM_DEBUG
        DEBUG_PRINTF("Alarm with ID %d not found for deletion.\n", alarmId);
        #endif
    }
}

int AlarmManager::getNextAlarmId() {
    int maxId = 0;
    for (const auto& alarm : m_alarms) {
        if (alarm.id > maxId) {
            maxId = alarm.id;
        }
    }
    return maxId + 1;
}



void AlarmManager::populateAlarmList() {
    #if ALARM_DEBUG
    DEBUG_PRINTLN("Populating alarm list UI...");
    #endif

    lv_obj_t* panel = get_alarms_panel_obj();
    if (!panel) {
        #if ALARM_DEBUG
        DEBUG_PRINTLN("Error: alarms_panel object not found!");
        #endif
        return;
    }

    clear_alarms_panel();
    deselectAlarm();

    for (const auto& alarm : m_alarms) {
        char days_str[50] = "";
        const char* day_initials[] = {"Mo", "Tu", "We", "Th", "Fr", "Sa", "Su"};
        bool day_found = false;
        if (alarm.repeat) {
            for(int i = 0; i < 7; i++) {
                if(alarm.weekdays[i]) {
                    if(day_found) {
                        strcat(days_str, ", ");
                    }
                    strcat(days_str, day_initials[i]);
                    day_found = true;
                }
            }
        }
        
        if (!day_found) {
            if (!alarm.date.isEmpty()) {
                strncpy(days_str, alarm.date.c_str(), sizeof(days_str) - 1);
            } else {
                strcpy(days_str, "Once");
            }
        }

        uint32_t child_cnt_before = lv_obj_get_child_cnt(panel);
        
        create_and_populate_alarm_widget(panel, alarm.title.c_str(), alarm.hour, alarm.minute, alarm.active, days_str);

        uint32_t child_cnt_after = lv_obj_get_child_cnt(panel);

        if (child_cnt_after > child_cnt_before) {
            lv_obj_t* new_widget = lv_obj_get_child(panel, child_cnt_after - 1);
            if (new_widget) {
                // Make the widget focusable on click
                lv_obj_add_flag(new_widget, LV_OBJ_FLAG_CLICK_FOCUSABLE);
                // Get the new focus handler
                extern void alarm_entry_focus_event_handler(lv_event_t * e);
                // Add events for both focus and defocus
                lv_obj_add_event_cb(new_widget, alarm_entry_focus_event_handler, LV_EVENT_FOCUSED, reinterpret_cast<void*>(static_cast<intptr_t>(alarm.id)));
                lv_obj_add_event_cb(new_widget, alarm_entry_focus_event_handler, LV_EVENT_DEFOCUSED, reinterpret_cast<void*>(static_cast<intptr_t>(alarm.id)));
            }
        }
    }

    #if ALARM_DEBUG
    DEBUG_PRINTF("Successfully populated %d alarm entries.\n", m_alarms.size());
    #endif
}

void AlarmManager::setSelectedAlarm(int alarmId, lv_obj_t* obj) {
    if (m_selectedAlarmObj) {
        lv_obj_remove_state(m_selectedAlarmObj, LV_STATE_CHECKED);
    }

    m_selectedAlarmId = alarmId;
    m_selectedAlarmObj = obj;

    if (m_selectedAlarmObj) {
        lv_obj_add_state(m_selectedAlarmObj, LV_STATE_CHECKED);
    }

    if (m_selectedAlarmId != -1) {
        lv_obj_clear_state(objects.edit_alarm_button, LV_STATE_DISABLED);
        lv_obj_clear_state(objects.delete_alarm_button, LV_STATE_DISABLED);
    } else {
        lv_obj_add_state(objects.edit_alarm_button, LV_STATE_DISABLED);
        lv_obj_add_state(objects.delete_alarm_button, LV_STATE_DISABLED);
    }
}

int AlarmManager::getSelectedAlarmId() const {
    return m_selectedAlarmId;
}

void AlarmManager::deleteSelectedAlarm() {
    if (m_selectedAlarmId != -1) {
        #if ALARM_DEBUG
        DEBUG_PRINTF("Deleting alarm %d.\n", m_selectedAlarmId);
        #endif
        deleteAlarm(m_selectedAlarmId);
        populateAlarmList();
    }
}

void AlarmManager::deselectAlarm() {
    setSelectedAlarm(-1, nullptr);
}
