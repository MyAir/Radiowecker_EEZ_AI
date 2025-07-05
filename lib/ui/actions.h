#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void action_eez_alarm_edit_screen_load_handler(lv_event_t * e);
extern void action_eez_save_alarm_button_handler(lv_event_t * e);
extern void action_eez_cancel_alarm_button_handler(lv_event_t * e);
extern void action_eez_add_alarm_button_handler(lv_event_t * e);
extern void action_eez_edit_alarm_button_handler(lv_event_t * e);
extern void action_eez_delete_alarm_button_handler(lv_event_t * e);
extern void action_eez_calendar_ok_button_handler(lv_event_t * e);
extern void action_eez_calendar_cancel_button_handler(lv_event_t * e);
extern void action_eez_show_calendar_button_handler(lv_event_t * e);
extern void action_eez_calendar_date_selected(lv_event_t * e);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/