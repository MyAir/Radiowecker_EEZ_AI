#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <lvgl.h>

// Event handlers for the alarm screen
void alarm_edit_screen_load_handler(lv_event_t *e);
void save_button_event_handler(lv_event_t *e);
void cancel_button_event_handler(lv_event_t *e);
void add_alarm_button_event_handler(lv_event_t *e);
void alarm_entry_click_event_handler(lv_event_t *e);
void edit_button_event_handler(lv_event_t *e);
void delete_button_event_handler(lv_event_t *e);

// Event handler for the alarm title text area to manage the keyboard
void alarm_title_textarea_event_handler(lv_event_t * e);

#endif // EVENT_HANDLER_H
