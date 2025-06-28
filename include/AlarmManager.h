#ifndef ALARM_MANAGER_H
#define ALARM_MANAGER_H

#include <Arduino.h>
#include <vector>
#include <lvgl.h>

// Struct to hold all properties of a single alarm
struct Alarm {
    int id;
    bool active;
    String title;
    int hour;
    int minute;
    bool repeat;
    bool weekdays[7]; // Monday to Sunday
    String date;      // YYYY-MM-DD for single-event alarms
};

enum class AlarmEditMode {
    NONE,
    ADD,
    EDIT
};

class AlarmManager {
public:
    // Get the singleton instance of the AlarmManager
    static AlarmManager* getInstance();

    // Save all alarms from memory to /data/alarms.json
    bool saveAlarms();

    // Dynamically create and populate the alarm widgets on the Alarms screen
    void populateAlarmList();

    // Get the list of alarms
    const std::vector<Alarm>& getAlarms() const;

    // Methods to manage a single alarm's data
    void saveAlarm(const Alarm& alarm);
    void deleteAlarm(int alarmId);
    int getNextAlarmId();

    // Methods to manage alarm editing state
    void startAddAlarm();
    void startEditAlarm(int alarmId);
    AlarmEditMode getEditMode() const;
    Alarm* getEditingAlarm();
    
    // Alarm selection management
    void setSelectedAlarm(int alarmId, lv_obj_t* obj);
    int getSelectedAlarmId() const;
    void deleteSelectedAlarm();
    void deselectAlarm();

private:
    AlarmManager();
    ~AlarmManager();
    AlarmManager(const AlarmManager&) = delete;
    AlarmManager& operator=(const AlarmManager&) = delete;

    void loadAlarms(); // Only private, called by constructor


    std::vector<Alarm> m_alarms;
    static AlarmManager* m_instance;
    AlarmEditMode m_editMode;
    int m_editingAlarmId;
    int m_selectedAlarmId;
    lv_obj_t* m_selectedAlarmObj;
};

#endif // ALARM_MANAGER_H
