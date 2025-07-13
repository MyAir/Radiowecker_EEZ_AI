#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *menu;
    lv_obj_t *alarms;
    lv_obj_t *alarm_edit_screen;
    lv_obj_t *radio;
    lv_obj_t *obj0;
    lv_obj_t *obj0__status_bar;
    lv_obj_t *obj0__wifi_label;
    lv_obj_t *obj0__ip_label;
    lv_obj_t *obj0__wifi_quality_label;
    lv_obj_t *obj1;
    lv_obj_t *obj1__playback_panel;
    lv_obj_t *obj1__playback_text_container;
    lv_obj_t *obj1__alarm_titile;
    lv_obj_t *obj1__title;
    lv_obj_t *obj1__album;
    lv_obj_t *obj1__artist;
    lv_obj_t *obj2;
    lv_obj_t *obj2__status_bar;
    lv_obj_t *obj2__wifi_label;
    lv_obj_t *obj2__ip_label;
    lv_obj_t *obj2__wifi_quality_label;
    lv_obj_t *obj3;
    lv_obj_t *obj3__status_bar;
    lv_obj_t *obj3__wifi_label;
    lv_obj_t *obj3__ip_label;
    lv_obj_t *obj3__wifi_quality_label;
    lv_obj_t *obj4;
    lv_obj_t *obj4__status_bar;
    lv_obj_t *obj4__wifi_label;
    lv_obj_t *obj4__ip_label;
    lv_obj_t *obj4__wifi_quality_label;
    lv_obj_t *obj5;
    lv_obj_t *obj5__status_bar;
    lv_obj_t *obj5__wifi_label;
    lv_obj_t *obj5__ip_label;
    lv_obj_t *obj5__wifi_quality_label;
    lv_obj_t *obj6;
    lv_obj_t *obj6__playback_panel;
    lv_obj_t *obj6__playback_text_container;
    lv_obj_t *obj6__alarm_titile;
    lv_obj_t *obj6__title;
    lv_obj_t *obj6__album;
    lv_obj_t *obj6__artist;
    lv_obj_t *content;
    lv_obj_t *back_button;
    lv_obj_t *alarms_menu_button;
    lv_obj_t *obj7;
    lv_obj_t *alarms_back_button;
    lv_obj_t *add_alarm_button;
    lv_obj_t *edit_alarm_button;
    lv_obj_t *delete_alarm_button;
    lv_obj_t *alarm_repeat_switch;
    lv_obj_t *show_calendar_button;
    lv_obj_t *save_alarm_button;
    lv_obj_t *cancel_alarm_button;
    lv_obj_t *calendar_selector;
    lv_obj_t *date_selection_ok_button;
    lv_obj_t *date_selection_cancel_button;
    lv_obj_t *radio_page_back_button;
    lv_obj_t *radio_station_list_dropdown;
    lv_obj_t *radio_volume_slider;
    lv_obj_t *radio_play_button;
    lv_obj_t *radio_stop_button;
    lv_obj_t *panel_content;
    lv_obj_t *time_panel;
    lv_obj_t *current_date;
    lv_obj_t *current_time;
    lv_obj_t *alarm_container;
    lv_obj_t *alarm_panel;
    lv_obj_t *alarm_off_button;
    lv_obj_t *next_alarm;
    lv_obj_t *sensor_panel;
    lv_obj_t *sensor_title_container;
    lv_obj_t *temp_title_container;
    lv_obj_t *temp_title;
    lv_obj_t *hum_title_container;
    lv_obj_t *hum_title;
    lv_obj_t *co2_title_container;
    lv_obj_t *co2_title;
    lv_obj_t *tvoc_title_container;
    lv_obj_t *tvoc_title;
    lv_obj_t *sensor_value_container;
    lv_obj_t *temp_value_container;
    lv_obj_t *temp_value;
    lv_obj_t *hum_value_container;
    lv_obj_t *hum_value;
    lv_obj_t *co2_value_container;
    lv_obj_t *co2_value;
    lv_obj_t *tvoc_value_container;
    lv_obj_t *tvoc_value;
    lv_obj_t *weather_panel;
    lv_obj_t *current_weather_container;
    lv_obj_t *current_weather_panel;
    lv_obj_t *current_weather_title_label;
    lv_obj_t *current_weather_icon;
    lv_obj_t *current_temp_label;
    lv_obj_t *feels_like_label;
    lv_obj_t *weather_desc_label;
    lv_obj_t *weather_forecast_container;
    lv_obj_t *morning_panel;
    lv_obj_t *morning_title_label;
    lv_obj_t *morning_icon;
    lv_obj_t *morning_temp_label;
    lv_obj_t *morning_rain_label;
    lv_obj_t *afternoon_panel;
    lv_obj_t *afternoon_title_label;
    lv_obj_t *afternoon_icon;
    lv_obj_t *afternoon_temp_label;
    lv_obj_t *afternoon_rain_label;
    lv_obj_t *night_panel;
    lv_obj_t *night_title_label;
    lv_obj_t *night_icon;
    lv_obj_t *night_temp_label;
    lv_obj_t *night_rain_label;
    lv_obj_t *menu_container;
    lv_obj_t *menu_items_content_container;
    lv_obj_t *button_panel;
    lv_obj_t *menu_content_container;
    lv_obj_t *menu_panel;
    lv_obj_t *alarms_container;
    lv_obj_t *alarm_menu_content_container;
    lv_obj_t *alarm_button_panel;
    lv_obj_t *alarm_maintain_panel;
    lv_obj_t *alarms_content_container;
    lv_obj_t *alarms_title_container;
    lv_obj_t *alarms_active_label_1;
    lv_obj_t *alarms_active_switch_1;
    lv_obj_t *alarms_panel;
    lv_obj_t *alarm_edit_screen_container;
    lv_obj_t *alarm_edit_screen_content_container;
    lv_obj_t *alarm_edit_container;
    lv_obj_t *alarm_edit_title_panel;
    lv_obj_t *alarm_edit_title_lable;
    lv_obj_t *alarm_edit_panel;
    lv_obj_t *alarm_title_textarea;
    lv_obj_t *time_container;
    lv_obj_t *time_selection_container;
    lv_obj_t *time_label_container;
    lv_obj_t *time_label;
    lv_obj_t *time_rollers_container;
    lv_obj_t *hour_roller;
    lv_obj_t *minute_roller;
    lv_obj_t *alarm_repeat_container;
    lv_obj_t *alarm_repeat_label;
    lv_obj_t *date_container;
    lv_obj_t *date_label_container;
    lv_obj_t *date_label;
    lv_obj_t *date_value_fields_container;
    lv_obj_t *date_value_container_1;
    lv_obj_t *alarm_date_label;
    lv_obj_t *weekday_container;
    lv_obj_t *weekday_label_container;
    lv_obj_t *weekday_label;
    lv_obj_t *weekday_value_fields_container;
    lv_obj_t *monday_container;
    lv_obj_t *monday_label_container;
    lv_obj_t *monday_label;
    lv_obj_t *monday_checkbox_container;
    lv_obj_t *monday_checkbox;
    lv_obj_t *tuesday_container;
    lv_obj_t *tuesday_label_container;
    lv_obj_t *tuesday_label;
    lv_obj_t *tuesday_checkbox_container;
    lv_obj_t *tuesday_checkbox;
    lv_obj_t *wednesday_container;
    lv_obj_t *wednesday_label_container;
    lv_obj_t *wednesday_label;
    lv_obj_t *wednesday_checkbox_container;
    lv_obj_t *wednesday_checkbox;
    lv_obj_t *thursday_container;
    lv_obj_t *thursday_label_container;
    lv_obj_t *thursday_label;
    lv_obj_t *thursday_checkbox_container;
    lv_obj_t *thursday_checkbox;
    lv_obj_t *friday_container;
    lv_obj_t *friday_label_container;
    lv_obj_t *friday_label;
    lv_obj_t *friday_checkbox_container;
    lv_obj_t *friday_checkbox;
    lv_obj_t *saturday_container;
    lv_obj_t *saturday_label_container;
    lv_obj_t *saturday_label;
    lv_obj_t *saturday_checkbox_container;
    lv_obj_t *saturday_checkbox;
    lv_obj_t *sunday_container;
    lv_obj_t *sunday_label_container;
    lv_obj_t *sunday_label;
    lv_obj_t *sunday_checkbox_container;
    lv_obj_t *sunday_checkbox;
    lv_obj_t *alarm_edit_button_panel;
    lv_obj_t *calendar_popup_panel;
    lv_obj_t *radio_container;
    lv_obj_t *radio_page_content_container;
    lv_obj_t *radio_page_button_panel;
    lv_obj_t *radio_content_container;
    lv_obj_t *radio_panel;
    lv_obj_t *obj8;
    lv_obj_t *playback_image;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_MENU = 2,
    SCREEN_ID_ALARMS = 3,
    SCREEN_ID_ALARM_EDIT_SCREEN = 4,
    SCREEN_ID_RADIO = 5,
};

void create_screen_main();
void tick_screen_main();

void create_screen_menu();
void tick_screen_menu();

void create_screen_alarms();
void tick_screen_alarms();

void create_screen_alarm_edit_screen();
void tick_screen_alarm_edit_screen();

void create_screen_radio();
void tick_screen_radio();

void create_user_widget_status_bar(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_status_bar(void *flowState, int startWidgetIndex);

void create_user_widget_alarm_entry(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_alarm_entry(void *flowState, int startWidgetIndex);

void create_user_widget_playback_panel(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_playback_panel(void *flowState, int startWidgetIndex);

enum Themes {
    THEME_ID_DARK,
    THEME_ID_LIGHT,
};
enum Colors {
    COLOR_ID_STATUS_BAR_BACKGROUND,
    COLOR_ID_STATUS_BAR_TEXT,
    COLOR_ID_PANEL_BORDER,
    COLOR_ID_PANEL_BACKGROUND,
    COLOR_ID_TIME_TEXT,
    COLOR_ID_TEXT,
    COLOR_ID_ALARM_PANEL,
    COLOR_ID_ALARM_TEXT,
    COLOR_ID_PANEL_WEATHER_CURRENT,
    COLOR_ID_PANEL_WEATHER_FORECAST,
    COLOR_ID_TEXT_WEATHER,
    COLOR_ID_ALARM_ENTRY_BACKGROUND,
    COLOR_ID_ALARM_ENTRY_BACKGROUND_FOCUSED,
    COLOR_ID_BUTTON_RED,
    COLOR_ID_BUTTON_GREEN,
    COLOR_ID_PLAYBACK_PANEL,
};
void change_color_theme(uint32_t themeIndex);
extern uint32_t theme_colors[2][16];

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/