#ifndef RADIO_DATA_H
#define RADIO_DATA_H

#include <Arduino.h>
#include <vector>
#include "lvgl.h"

// Represents a single radio station
struct Station {
    int id;
    String name;
    String url;
    String genre;
};

// Represents the currently playing track information
// This will be bound to the UI
struct PlaybackInfo {
    char AlarmTitle[64];
    char Title[128];
    char Album[64];
    char Artist[64];
};

// Global vector to hold all radio stations loaded from stations.json
extern std::vector<Station> g_stations;

// Global struct for playback info, for UI data binding
extern PlaybackInfo g_playbackInfo;

// Helper function to populate the station list for the UI dropdown
void populate_station_list_for_ui(lv_obj_t *dropdown);

#endif // RADIO_DATA_H
