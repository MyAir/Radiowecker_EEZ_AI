#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// enum declarations



// Flow global variables

enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_CURRENT_WEATHER = 0,
    FLOW_GLOBAL_VARIABLE_CURRENT_TIME = 1,
    FLOW_GLOBAL_VARIABLE_CURRENT_DATE = 2,
    FLOW_GLOBAL_VARIABLE_WIFI_SSID = 3,
    FLOW_GLOBAL_VARIABLE_WIFI_IP = 4,
    FLOW_GLOBAL_VARIABLE_WIFI_QUALITY = 5,
    FLOW_GLOBAL_VARIABLE_PLAYBACK_INFO = 6,
    FLOW_GLOBAL_VARIABLE_STATION_LIST = 7
};

// Native global variables



#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/