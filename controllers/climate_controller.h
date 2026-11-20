#ifndef CLIMATE_CONTROLLER_H
#define CLIMATE_CONTROLLER_H

#include "../devices/temperature_sensor.h"
#include "../devices/ac.h"
#include "motion_controller.h"

#define MAX_AC_LINKS 16
#define TEMPERATURE_MARGIN 1.0f

typedef enum {
    CLIMATE_CONTROL_ON,
    CLIMATE_CONTROL_OFF
} ClimateControlState;

typedef struct {
    TemperatureSensor *temperature_sensor;

    /* Optional occupancy input */
    MotionController *motion_controller;

    AC *ac[MAX_AC_LINKS];
    int ac_link_count;

    ClimateControlState state;
    float target_temperature;
} ClimateController;

void ClimateController_Init(ClimateController *controller, TemperatureSensor *temperature_sensor);

void ClimateController_LinkMotion(ClimateController *controller, MotionController *motion_controller);
void ClimateController_AddACLink(ClimateController *controller, AC *ac);

void ClimateController_SetTargetTemperature(ClimateController *controller, float target_temperature);
float ClimateController_GetTargetTemperature(const ClimateController *controller);

void ClimateController_Update(ClimateController *controller);

#endif