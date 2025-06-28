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
    lv_obj_t *obj0;
    lv_obj_t *obj0__status_bar;
    lv_obj_t *obj0__wifi_label;
    lv_obj_t *obj0__ip_label;
    lv_obj_t *obj0__wifi_quality_label;
    lv_obj_t *obj1;
    lv_obj_t *obj1__status_bar;
    lv_obj_t *obj1__wifi_label;
    lv_obj_t *obj1__ip_label;
    lv_obj_t *obj1__wifi_quality_label;
    lv_obj_t *obj2;
    lv_obj_t *obj2__status_bar;
    lv_obj_t *obj2__wifi_label;
    lv_obj_t *obj2__ip_label;
    lv_obj_t *obj2__wifi_quality_label;
    lv_obj_t *alarm_entry_1;
    lv_obj_t *alarm_entry_1__alarm_entry_panel;
    lv_obj_t *alarm_entry_1__alarm_entry_container;
    lv_obj_t *alarm_entry_2;
    lv_obj_t *alarm_entry_2__alarm_entry_panel;
    lv_obj_t *alarm_entry_2__alarm_entry_container;
    lv_obj_t *alarm_entry_3;
    lv_obj_t *alarm_entry_3__alarm_entry_panel;
    lv_obj_t *alarm_entry_3__alarm_entry_container;
    lv_obj_t *content;
    lv_obj_t *back_button;
    lv_obj_t *alarms_menu_button;
    lv_obj_t *back_button_1;
    lv_obj_t *panel_content;
    lv_obj_t *time_panel;
    lv_obj_t *current_date;
    lv_obj_t *current_time;
    lv_obj_t *alarm_container;
    lv_obj_t *alarm_panel;
    lv_obj_t *alarm_text_container;
    lv_obj_t *alarm_titile;
    lv_obj_t *title;
    lv_obj_t *album;
    lv_obj_t *artist;
    lv_obj_t *alarm_off_container;
    lv_obj_t *alarm_off;
    lv_obj_t *album_art_container;
    lv_obj_t *album_art;
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
    lv_obj_t *alarms_content_container;
    lv_obj_t *alarms_title_container;
    lv_obj_t *alarms_active_label_1;
    lv_obj_t *alarms_active_switch_1;
    lv_obj_t *alarms_panel;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_MENU = 2,
    SCREEN_ID_ALARMS = 3,
};

void create_screen_main();
void tick_screen_main();

void create_screen_menu();
void tick_screen_menu();

void create_screen_alarms();
void tick_screen_alarms();

void create_user_widget_status_bar(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_status_bar(void *flowState, int startWidgetIndex);

void create_user_widget_alarm_entry(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_alarm_entry(void *flowState, int startWidgetIndex);

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
};
void change_color_theme(uint32_t themeIndex);
extern uint32_t theme_colors[2][12];

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/