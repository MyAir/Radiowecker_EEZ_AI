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
    FLOW_STRUCTURE_DATE_STRUC = 16385
};

enum FlowArrayOfStructures {
    FLOW_ARRAY_OF_STRUCTURE_WEATHER = 81920,
    FLOW_ARRAY_OF_STRUCTURE_DATE_STRUC = 81921
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


#endif

#endif /*EEZ_LVGL_UI_STRUCTS_H*/
