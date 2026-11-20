#ifndef ROOM_H
#define ROOM_H

#include "../devices/light.h"
#include "../devices/motion_detector.h"
#include "../devices/smoke_detector.h"
#include "../devices/temperature_sensor.h"
#include "../devices/ac.h"
#include "../devices/sprinkler.h"

#include "../controllers/motion_controller.h"
#include "../controllers/smoke_controller.h"
#include "../controllers/climate_controller.h"

#include <stdbool.h>

#define MAX_DEVICES_PER_ROOM 4

typedef struct {
    Light light[MAX_DEVICES_PER_ROOM];
    int light_count;

    MotionDetector motion_detector[MAX_DEVICES_PER_ROOM];
    int motion_detector_count;

    SmokeDetector smoke_detector;
    bool has_smoke_detector;

    TemperatureSensor temperature_sensor[MAX_DEVICES_PER_ROOM];
    int temperature_sensor_count;

    AC ac[MAX_DEVICES_PER_ROOM];
    int ac_count;

    Sprinkler sprinkler[MAX_DEVICES_PER_ROOM];
    int sprinkler_count;

    MotionController motion_controllers[MAX_DEVICES_PER_ROOM];

    SmokeController smoke_controller;

    ClimateController climate_controllers[MAX_DEVICES_PER_ROOM];
    int climate_controller_count;
} Room;

void Room_Init(Room *room);

int add_light(Room *room, int gpio_pin);
int add_motion_detector(Room *room, int gpio_pin);
int add_smoke_detector(Room *room, int gpio_pin);
int add_temperature_sensor(Room *room);
int add_ac(Room *room, int gpio_pin);
int add_sprinkler(Room *room, int gpio_pin);

int add_climate_controller(Room *room, int temperature_index);

void link_motion_to_light(Room *room, int motion_index, int light_index);
void link_motion_to_climate(Room *room, int motion_index, int climate_index);
void link_smoke_to_sprinkler(Room *room, int sprinkler_index);
void link_climate_to_ac(Room *room, int climate_index, int ac_index);

void Room_Update(Room *room);
bool Room_IsOccupied(const Room *room);
bool Room_HasSmokeAlert(const Room *room);
float Room_GetAverageTemperature(const Room *room);
void Room_TurnAllLightsOff(Room *room);

#endif