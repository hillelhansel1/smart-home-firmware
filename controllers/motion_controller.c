#include "motion_controller.h"
#include <stddef.h>

static void TurnLightsOn(MotionController *controller);
static void TurnLightsOff(MotionController *controller);

void MotionController_Init(MotionController *controller, MotionDetector *motion_detector){
    if(controller == NULL || motion_detector == NULL){
        return;
    }
    controller->motion_detector = motion_detector;
    controller->state = MOTION_CONTROL_OFF;
    controller->light_link_count = 0;
    controller->waiting_start_time = 0;
}

void MotionController_AddLightLink(MotionController *controller, Light *light){
    if(controller == NULL || light == NULL){
        return;
    }
    if(controller->light_link_count >= MAX_LINKS_PER_CONTROLLER){
        return;
    }
    controller->light_links[controller->light_link_count] = light;
    controller->light_link_count++;
}

void MotionController_Update(MotionController *controller, uint64_t current_time){
    if(controller == NULL || controller->motion_detector == NULL){
        return;
    }
    bool has_event = MotionDetector_HasEvent(controller->motion_detector);
    MotionState motion_state = MOTION_NOT_DETECTED;

    if(has_event){
        motion_state = MotionDetector_Read(controller->motion_detector);
        MotionDetector_ClearEvent(controller->motion_detector);
    }

    switch(controller->state){
        case MOTION_CONTROL_ON:
            if(has_event && motion_state == MOTION_NOT_DETECTED){
                controller->state = MOTION_WAITING_OFF;
                controller->waiting_start_time = current_time;
            }
            break;
        
        case MOTION_CONTROL_OFF:
            if(has_event && motion_state == MOTION_DETECTED){
                controller->state = MOTION_CONTROL_ON;
                TurnLightsOn(controller);
            }
            break;
        
        case MOTION_WAITING_OFF:
            if(has_event && motion_state == MOTION_DETECTED){
                controller->state = MOTION_CONTROL_ON;
                TurnLightsOn(controller);
            }

            else{
                uint64_t elapsed = current_time - controller->waiting_start_time;
                if(elapsed >= LIGHT_MOTION_TIMEOUT_MS){
                    TurnLightsOff(controller);
                    controller->state = MOTION_CONTROL_OFF;
                }
            }
            break;
        
        default:
            break;
    }
}

bool MotionController_IsOccupied(const MotionController *controller){
    if(controller == NULL){
        return false;
    }

    return controller->state != MOTION_CONTROL_OFF;
}

static void TurnLightsOn(MotionController *controller){
    for(int i = 0; i < controller->light_link_count; i++){
        Light_TurnOn(controller->light_links[i]);
    }
}

static void TurnLightsOff(MotionController *controller){
    for(int i = 0; i < controller->light_link_count; i++){
        Light_TurnOff(controller->light_links[i]);
    }
}