#include "RadioData.h"
#include "ui.h"
#include <vector>

// Define the global station vector and playback info struct
std::vector<Station> g_stations;
PlaybackInfo g_playbackInfo;

// Populates the LVGL dropdown with station names
void populate_station_list_for_ui(lv_obj_t *dropdown) {
    if (!dropdown) return;

    String station_options = "";
    for (const auto& station : g_stations) {
        if (!station_options.isEmpty()) {
            station_options += "\n";
        }
        station_options += station.name;
    }
    
    lv_dropdown_set_options(dropdown, station_options.c_str());
}
