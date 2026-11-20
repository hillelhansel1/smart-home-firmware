#include "temperature_sensor.h"
#include <stddef.h>

void TemperatureSensor_Init(TemperatureSensor *sensor){
    if(sensor == NULL){
        return;
    }
    sensor->simulated_temperature = 22.0f;
}

float TemperatureSensor_Read(const TemperatureSensor *sensor){
    if(sensor == NULL){
        return 0;
    }
    return sensor->simulated_temperature;
}

void TemperatureSensor_Set(TemperatureSensor *sensor, float temperature){
    if(sensor == NULL){
        return;
    }
    sensor->simulated_temperature = temperature;
}