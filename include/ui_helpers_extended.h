#ifndef UI_HELPERS_EXTENDED_H
#define UI_HELPERS_EXTENDED_H

#include <lvgl.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Gets a pointer to the main alarms panel container
lv_obj_t *get_alarms_panel_obj();

// Clears all children from the alarms panel
void clear_alarms_panel();

void create_and_populate_alarm_widget(
    lv_obj_t *parent,
    const char *title,
    int hour,
    int minute,
    bool active,
    const char *date_or_weekdays
);

void create_alarm_widget(const char *time, const char *period, const char *date, const char *repeat, bool enabled);

#ifdef __cplusplus
}
#endif

#endif // UI_HELPERS_EXTENDED_H
