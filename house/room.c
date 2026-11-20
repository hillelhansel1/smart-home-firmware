#include "room.h"
#include "../system/system_time.h"

#include <stddef.h>

void Room_Init(Room *room)
{
    if(room == NULL){
        return;
    }

    room->light_count = 0;
    room->motion_detector_count = 0;
    room->has_smoke_detector = false;
    room->temperature_sensor_count = 0;
    room->ac_count = 0;
    room->sprinkler_count = 0;
    room->climate_controller_count = 0;
}

int add_light(Room *room, int gpio_pin)
{
    if(room == NULL || room->light_count >= MAX_DEVICES_PER_ROOM){
        return -1;
    }

    int index = room->light_count;

    if(!Light_Init(&room->light[index], gpio_pin)){
        return -1;
    }

    room->light_count++;
    return index;
}

int add_motion_detector(Room *room, int gpio_pin)
{
    if(room == NULL || room->motion_detector_count >= MAX_DEVICES_PER_ROOM){
        return -1;
    }

    int index = room->motion_detector_count;

    if(!MotionDetector_Init(&room->motion_detector[index], gpio_pin)){
        return -1;
    }

    MotionController_Init(&room->motion_controllers[index], &room->motion_detector[index]);

    room->motion_detector_count++;
    return index;
}

int add_smoke_detector(Room *room, int gpio_pin)
{
    if(room == NULL || room->has_smoke_detector){
        return -1;
    }

    if(!SmokeDetector_Init(&room->smoke_detector, gpio_pin)){
        return -1;
    }

    SmokeController_Init(&room->smoke_controller, &room->smoke_detector);

    room->has_smoke_detector = true;
    return 0;
}

int add_temperature_sensor(Room *room)
{
    if(room == NULL || room->temperature_sensor_count >= MAX_DEVICES_PER_ROOM){
        return -1;
    }

    int index = room->temperature_sensor_count;

    TemperatureSensor_Init(&room->temperature_sensor[index]);

    room->temperature_sensor_count++;
    return index;
}

int add_ac(Room *room, int gpio_pin)
{
    if(room == NULL || room->ac_count >= MAX_DEVICES_PER_ROOM){
        return -1;
    }

    int index = room->ac_count;

    if(!AC_Init(&room->ac[index], gpio_pin)){
        return -1;
    }

    room->ac_count++;
    return index;
}

int add_sprinkler(Room *room, int gpio_pin)
{
    if(room == NULL || room->sprinkler_count >= MAX_DEVICES_PER_ROOM){
        return -1;
    }

    int index = room->sprinkler_count;

    if(!Sprinkler_Init(&room->sprinkler[index], gpio_pin)){
        return -1;
    }

    room->sprinkler_count++;
    return index;
}

int add_climate_controller(Room *room, int temperature_index)
{
    if(room == NULL){
        return -1;
    }

    if(temperature_index < 0 || temperature_index >= room->temperature_sensor_count){
        return -1;
    }

    if(room->climate_controller_count >= MAX_DEVICES_PER_ROOM){
        return -1;
    }

    int index = room->climate_controller_count;

    ClimateController_Init(&room->climate_controllers[index], &room->temperature_sensor[temperature_index]);

    room->climate_controller_count++;
    return index;
}

void link_motion_to_light(Room *room, int motion_index, int light_index)
{
    if(room == NULL){
        return;
    }

    if(motion_index < 0 || motion_index >= room->motion_detector_count){
        return;
    }

    if(light_index < 0 || light_index >= room->light_count){
        return;
    }

    MotionController_AddLightLink(&room->motion_controllers[motion_index], &room->light[light_index]);
}

void link_motion_to_climate(Room *room, int motion_index, int climate_index){
    if(room == NULL){
        return;
    }

    if(motion_index < 0 || motion_index >= room->motion_detector_count){
        return;
    }

    if(climate_index < 0 || climate_index >= room->climate_controller_count){
        return;
    }

    ClimateController_LinkMotion(&room->climate_controllers[climate_index], &room->motion_controllers[motion_index]);
}

void link_smoke_to_sprinkler(Room *room, int sprinkler_index)
{
    if(room == NULL || !room->has_smoke_detector){
        return;
    }

    if(sprinkler_index < 0 || sprinkler_index >= room->sprinkler_count){
        return;
    }

    SmokeController_AddSprinklerLink(&room->smoke_controller, &room->sprinkler[sprinkler_index]);
}

void link_climate_to_ac(Room *room, int climate_index, int ac_index)
{
    if(room == NULL){
        return;
    }

    if(climate_index < 0 || climate_index >= room->climate_controller_count){
        return;
    }

    if(ac_index < 0 || ac_index >= room->ac_count){
        return;
    }

    ClimateController_AddACLink(&room->climate_controllers[climate_index], &room->ac[ac_index]);
}

void Room_Update(Room *room)
{
    if(room == NULL){
        return;
    }

    uint64_t current_time = Time_GetMilliseconds();

    if(room->has_smoke_detector){
        SmokeController_Update(&room->smoke_controller, current_time);
    }

    for(int i = 0; i < room->motion_detector_count; i++){
        MotionController_Update(&room->motion_controllers[i], current_time);
    }

    for(int i = 0; i < room->climate_controller_count; i++){
        ClimateController_Update(&room->climate_controllers[i]);
    }
}

bool Room_IsOccupied(const Room *room){
    if(room == NULL){
        return false;
    }
    for(int i = 0; i < room->motion_detector_count; i++){
        if(MotionController_IsOccupied(&(room->motion_controllers[i]))){
            return true;
        }
    }

    return false;
}

bool Room_HasSmokeAlert(const Room *room){
    if(room == NULL || !room->has_smoke_detector){
        return false;
    }

    return SmokeController_GetState(&room->smoke_controller) == SMOKE_CONTROL_ON;
}

float Room_GetAverageTemperature(const Room *room){
    if(room == NULL || room->temperature_sensor_count == 0){
        return 0.0f;
    }

    float sum = 0.0f;

    for(int i = 0; i < room->temperature_sensor_count; i++){
        sum += TemperatureSensor_Read(&room->temperature_sensor[i]);
    }

    return sum / room->temperature_sensor_count;
}

void Room_TurnAllLightsOff(Room *room){
    if(room == NULL){
        return;
    }

    for(int i = 0; i < room->light_count; i++){
        Light_TurnOff(&(room->light[i]));
    }
}