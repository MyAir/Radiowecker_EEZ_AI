#include "ui_helpers_extended.h"
#include "screens.h" // Correct header for objects and screen creation functions

lv_obj_t *get_alarms_panel_obj() {
    // The UI is initialized in main.cpp long before this is called,
    // so we can safely access the objects directly.
    return objects.alarms_panel;
}

void clear_alarms_panel() {
    lv_obj_t *parent = get_alarms_panel_obj();
    if (parent) {
        lv_obj_clean(parent);
    }
}
#include <stdio.h>
#include <string.h>

void create_and_populate_alarm_widget(
    lv_obj_t *parent,
    const char *title,
    int hour,
    int minute,
    bool active,
    const char *date_or_weekdays
) {
    // This is a workaround for the EEZ Studio code generation.
    // The create_user_widget function writes to the global `objects` struct, which would corrupt pointers
    // to other UI elements. To avoid this, we backup the start of the `objects` struct, let the function
    // write into it, copy the pointers we need, and then restore the backup.
    const int NUM_WIDGETS_IN_ALARM_ENTRY = 21;
    lv_obj_t* object_backup[NUM_WIDGETS_IN_ALARM_ENTRY];

    // 1. Backup the portion of the global `objects` struct that will be overwritten.
    memcpy(object_backup, &objects, sizeof(object_backup));

    // 2. Create the alarm_entry user widget. We pass a startWidgetIndex of 0, so it writes
    //    into the memory we just backed up.
    create_user_widget_alarm_entry(parent, NULL, 0);

    // 3. Capture the pointers to the new widgets from the now-modified `objects` struct.
    //    The indices are based on the creation order in `create_user_widget_alarm_entry`.
    lv_obj_t* alarm_title_label = ((lv_obj_t **)&objects)[5];
    lv_obj_t* alarm_active_switch = ((lv_obj_t **)&objects)[8];
    lv_obj_t* alarm_time_label = ((lv_obj_t **)&objects)[14];
    lv_obj_t* alarm_repeat_info_label = ((lv_obj_t **)&objects)[20];

    // 4. Restore the global `objects` struct to its original state.
    memcpy(&objects, object_backup, sizeof(object_backup));

    // 5. Populate the newly created widget with alarm data.
    lv_label_set_text(alarm_title_label, title);

    char time_buf[6];
    snprintf(time_buf, sizeof(time_buf), "%02d:%02d", hour, minute);
    lv_label_set_text(alarm_time_label, time_buf);

    if (active) {
        lv_obj_add_state(alarm_active_switch, LV_STATE_CHECKED);
    } else {
        lv_obj_clear_state(alarm_active_switch, LV_STATE_CHECKED);
    }

    lv_label_set_text(alarm_repeat_info_label, date_or_weekdays);
}
