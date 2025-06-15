#ifndef EEZ_LVGL_UI_STRUCTS_H
#define EEZ_LVGL_UI_STRUCTS_H

#include "eez-flow.h"


#if defined(EEZ_FOR_LVGL)

#include <stdint.h>
#include <stdbool.h>

#include "vars.h"

using namespace eez;

enum FlowStructures {
    FLOW_STRUCTURE_WEATHER = 16384
};

enum FlowArrayOfStructures {
    FLOW_ARRAY_OF_STRUCTURE_WEATHER = 81920
};

enum WeatherFlowStructureFields {
    FLOW_STRUCTURE_WEATHER_FIELD_DT = 0,
    FLOW_STRUCTURE_WEATHER_FIELD_TEMPERATURE = 1,
    FLOW_STRUCTURE_WEATHER_NUM_FIELDS
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
    
    float Temperature() {
        return value.getArray()->values[FLOW_STRUCTURE_WEATHER_FIELD_TEMPERATURE].getFloat();
    }
    void Temperature(float Temperature) {
        value.getArray()->values[FLOW_STRUCTURE_WEATHER_FIELD_TEMPERATURE] = FloatValue(Temperature);
    }
};

typedef ArrayOf<WeatherValue, FLOW_ARRAY_OF_STRUCTURE_WEATHER> ArrayOfWeatherValue;


#endif

#endif /*EEZ_LVGL_UI_STRUCTS_H*/
