#include "climate_controller.h"
#include <stdlib.h>

static void TurnAcOn(ClimateController *controller);
static void TurnAcOff(ClimateController *controller);

void ClimateController_Init(ClimateController *controller, TemperatureSensor *temperature_sensor){
    if(controller == NULL || temperature_sensor == NULL){
        return;
    }
    controller->temperature_sensor = temperature_sensor;
    controller->motion_controller = NULL;
    controller->ac_link_count = 0;
    controller->state = CLIMATE_CONTROL_OFF;
    controller->target_temperature = 22.0f;
}

void ClimateController_LinkMotion(ClimateController *controller, MotionController *motion_controller){
    if(controller == NULL || motion_controller == NULL){
        return;
    }
    controller->motion_controller = motion_controller;
}

void ClimateController_AddACLink(ClimateController *controller, AC *ac){
    if(controller == NULL || ac == NULL){
        return;
    }

    if(controller->ac_link_count >= MAX_AC_LINKS){
        return;
    }
    
    controller->ac[controller->ac_link_count] = ac;
    controller->ac_link_count++;
}

void ClimateController_Update(ClimateController *controller)
{
    if(controller == NULL || controller->temperature_sensor == NULL){
        return;
    }

    float sense_temp = TemperatureSensor_Read(controller->temperature_sensor);

    if(controller->motion_controller != NULL){
        bool occupied = MotionController_IsOccupied(controller->motion_controller);

        if(!occupied){
            TurnAcOff(controller);
            controller->state = CLIMATE_CONTROL_OFF;
            return;
        }
    }

    switch(controller->state){

        case CLIMATE_CONTROL_ON:
            if(sense_temp <= controller->target_temperature - TEMPERATURE_MARGIN){
                TurnAcOff(controller);
                controller->state = CLIMATE_CONTROL_OFF;
            }
            break;

        case CLIMATE_CONTROL_OFF:
            if(sense_temp >= controller->target_temperature + TEMPERATURE_MARGIN){

                TurnAcOn(controller);
                controller->state = CLIMATE_CONTROL_ON;
            }
            break;

        default:
            break;
    }
}

void ClimateController_SetTargetTemperature(ClimateController *controller, float target_temperature){
    if(controller == NULL){
        return;
    }
    controller->target_temperature = target_temperature;
}

float ClimateController_GetTargetTemperature(const ClimateController *controller){
    if(controller == NULL){
        return 0.0f;
    }
    return controller->target_temperature;
}

static void TurnAcOn(ClimateController *controller){
    for(int i = 0; i < controller->ac_link_count; i++){
        AC_TurnOn(controller->ac[i]);
    }
}

static void TurnAcOff(ClimateController *controller){
    for(int i = 0; i < controller->ac_link_count; i++){
        AC_TurnOff(controller->ac[i]);
    }
}