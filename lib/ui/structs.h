#ifndef EEZ_LVGL_UI_STRUCTS_H
#define EEZ_LVGL_UI_STRUCTS_H

#include "eez-flow.h"


#if defined(EEZ_FOR_LVGL)

#include <stdint.h>
#include <stdbool.h>

#include "vars.h"

using namespace eez;

enum FlowStructures {
    FLOW_STRUCTURE_WEATHER = 16384,
    FLOW_STRUCTURE_DATE_STRUC = 16385,
    FLOW_STRUCTURE_PLAYBACK_STRUC = 16386,
    FLOW_STRUCTURE_STATION = 16387
};

enum FlowArrayOfStructures {
    FLOW_ARRAY_OF_STRUCTURE_WEATHER = 81920,
    FLOW_ARRAY_OF_STRUCTURE_DATE_STRUC = 81921,
    FLOW_ARRAY_OF_STRUCTURE_PLAYBACK_STRUC = 81922,
    FLOW_ARRAY_OF_STRUCTURE_STATION = 81923
};

enum WeatherFlowStructureFields {
    FLOW_STRUCTURE_WEATHER_FIELD_DT = 0,
    FLOW_STRUCTURE_WEATHER_FIELD_TEMPERATURE = 1,
    FLOW_STRUCTURE_WEATHER_NUM_FIELDS
};

enum DateStrucFlowStructureFields {
    FLOW_STRUCTURE_DATE_STRUC_FIELD_DAY = 0,
    FLOW_STRUCTURE_DATE_STRUC_FIELD_MONTH = 1,
    FLOW_STRUCTURE_DATE_STRUC_FIELD_YEAR = 2,
    FLOW_STRUCTURE_DATE_STRUC_NUM_FIELDS
};

enum PlaybackStrucFlowStructureFields {
    FLOW_STRUCTURE_PLAYBACK_STRUC_FIELD_ALARM_TITLE = 0,
    FLOW_STRUCTURE_PLAYBACK_STRUC_FIELD_TITLE = 1,
    FLOW_STRUCTURE_PLAYBACK_STRUC_FIELD_ALBUM = 2,
    FLOW_STRUCTURE_PLAYBACK_STRUC_FIELD_ARTIST = 3,
    FLOW_STRUCTURE_PLAYBACK_STRUC_NUM_FIELDS
};

enum StationFlowStructureFields {
    FLOW_STRUCTURE_STATION_FIELD_NAME = 0,
    FLOW_STRUCTURE_STATION_FIELD_URL = 1,
    FLOW_STRUCTURE_STATION_FIELD_FAVORITE = 2,
    FLOW_STRUCTURE_STATION_NUM_FIELDS
};

struct WeatherValue {
    Value value;
    
    WeatherValue() {
        value = Value::makeArrayRef(FLOW_STRUCTURE_WEATHER_NUM_FIELDS, FLOW_STRUCTURE_WEATHER, 0);
    }
    
    WeatherValue(Value value) : value(value) {}
    
    operator Value() const { return value; }
    
    operator bool() const { return value.isArray(); }
    
    int dt() {
        return value.getArray()->values[FLOW_STRUCTURE_WEATHER_FIELD_DT].getInt();
    }
    void dt(int dt) {
        value.getArray()->values[FLOW_STRUCTURE_WEATHER_FIELD_DT] = IntegerValue(dt);
    }
    
    const char *Temperature() {
        return value.getArray()->values[FLOW_STRUCTURE_WEATHER_FIELD_TEMPERATURE].getString();
    }
    void Temperature(const char *Temperature) {
        value.getArray()->values[FLOW_STRUCTURE_WEATHER_FIELD_TEMPERATURE] = StringValue(Temperature);
    }
};

typedef ArrayOf<WeatherValue, FLOW_ARRAY_OF_STRUCTURE_WEATHER> ArrayOfWeatherValue;
struct DateStrucValue {
    Value value;
    
    DateStrucValue() {
        value = Value::makeArrayRef(FLOW_STRUCTURE_DATE_STRUC_NUM_FIELDS, FLOW_STRUCTURE_DATE_STRUC, 0);
    }
    
    DateStrucValue(Value value) : value(value) {}
    
    operator Value() const { return value; }
    
    operator bool() const { return value.isArray(); }
    
    int Day() {
        return value.getArray()->values[FLOW_STRUCTURE_DATE_STRUC_FIELD_DAY].getInt();
    }
    void Day(int Day) {
        value.getArray()->values[FLOW_STRUCTURE_DATE_STRUC_FIELD_DAY] = IntegerValue(Day);
    }
    
    int Month() {
        return value.getArray()->values[FLOW_STRUCTURE_DATE_STRUC_FIELD_MONTH].getInt();
    }
    void Month(int Month) {
        value.getArray()->values[FLOW_STRUCTURE_DATE_STRUC_FIELD_MONTH] = IntegerValue(Month);
    }
    
    int Year() {
        return value.getArray()->values[FLOW_STRUCTURE_DATE_STRUC_FIELD_YEAR].getInt();
    }
    void Year(int Year) {
        value.getArray()->values[FLOW_STRUCTURE_DATE_STRUC_FIELD_YEAR] = IntegerValue(Year);
    }
};

typedef ArrayOf<DateStrucValue, FLOW_ARRAY_OF_STRUCTURE_DATE_STRUC> ArrayOfDateStrucValue;
struct PlaybackStrucValue {
    Value value;
    
    PlaybackStrucValue() {
        value = Value::makeArrayRef(FLOW_STRUCTURE_PLAYBACK_STRUC_NUM_FIELDS, FLOW_STRUCTURE_PLAYBACK_STRUC, 0);
    }
    
    PlaybackStrucValue(Value value) : value(value) {}
    
    operator Value() const { return value; }
    
    operator bool() const { return value.isArray(); }
    
    const char *AlarmTitle() {
        return value.getArray()->values[FLOW_STRUCTURE_PLAYBACK_STRUC_FIELD_ALARM_TITLE].getString();
    }
    void AlarmTitle(const char *AlarmTitle) {
        value.getArray()->values[FLOW_STRUCTURE_PLAYBACK_STRUC_FIELD_ALARM_TITLE] = StringValue(AlarmTitle);
    }
    
    const char *Title() {
        return value.getArray()->values[FLOW_STRUCTURE_PLAYBACK_STRUC_FIELD_TITLE].getString();
    }
    void Title(const char *Title) {
        value.getArray()->values[FLOW_STRUCTURE_PLAYBACK_STRUC_FIELD_TITLE] = StringValue(Title);
    }
    
    const char *Album() {
        return value.getArray()->values[FLOW_STRUCTURE_PLAYBACK_STRUC_FIELD_ALBUM].getString();
    }
    void Album(const char *Album) {
        value.getArray()->values[FLOW_STRUCTURE_PLAYBACK_STRUC_FIELD_ALBUM] = StringValue(Album);
    }
    
    const char *Artist() {
        return value.getArray()->values[FLOW_STRUCTURE_PLAYBACK_STRUC_FIELD_ARTIST].getString();
    }
    void Artist(const char *Artist) {
        value.getArray()->values[FLOW_STRUCTURE_PLAYBACK_STRUC_FIELD_ARTIST] = StringValue(Artist);
    }
};

typedef ArrayOf<PlaybackStrucValue, FLOW_ARRAY_OF_STRUCTURE_PLAYBACK_STRUC> ArrayOfPlaybackStrucValue;
struct StationValue {
    Value value;
    
    StationValue() {
        value = Value::makeArrayRef(FLOW_STRUCTURE_STATION_NUM_FIELDS, FLOW_STRUCTURE_STATION, 0);
    }
    
    StationValue(Value value) : value(value) {}
    
    operator Value() const { return value; }
    
    operator bool() const { return value.isArray(); }
    
    const char *name() {
        return value.getArray()->values[FLOW_STRUCTURE_STATION_FIELD_NAME].getString();
    }
    void name(const char *name) {
        value.getArray()->values[FLOW_STRUCTURE_STATION_FIELD_NAME] = StringValue(name);
    }
    
    const char *url() {
        return value.getArray()->values[FLOW_STRUCTURE_STATION_FIELD_URL].getString();
    }
    void url(const char *url) {
        value.getArray()->values[FLOW_STRUCTURE_STATION_FIELD_URL] = StringValue(url);
    }
    
    bool favorite() {
        return value.getArray()->values[FLOW_STRUCTURE_STATION_FIELD_FAVORITE].getBoolean();
    }
    void favorite(bool favorite) {
        value.getArray()->values[FLOW_STRUCTURE_STATION_FIELD_FAVORITE] = BooleanValue(favorite);
    }
};

typedef ArrayOf<StationValue, FLOW_ARRAY_OF_STRUCTURE_STATION> ArrayOfStationValue;


#endif

#endif /*EEZ_LVGL_UI_STRUCTS_H*/
