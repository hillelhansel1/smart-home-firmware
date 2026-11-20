#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

typedef struct{
    float simulated_temperature;
} TemperatureSensor;

void TemperatureSensor_Init(TemperatureSensor *sensor);

float TemperatureSensor_Read(const TemperatureSensor *sensor);

void TemperatureSensor_Set(TemperatureSensor *sensor, float temperature);

#endif