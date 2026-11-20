#include "smoke_controller.h"
#include <stdlib.h>

static void TurnSprinklersOn(SmokeController *controller);
static void TurnSprinklersOff(SmokeController *controller);

void SmokeController_Init(SmokeController *controller, SmokeDetector *smoke_detector){
    if(controller == NULL || smoke_detector == NULL){
        return;
    }
    controller->smoke_detector = smoke_detector;
    controller->state = SMOKE_CONTROL_OFF;
    controller->sprinkler_count = 0;
    controller->waiting_start_time = 0;
}

void SmokeController_AddSprinklerLink(SmokeController *controller, Sprinkler *sprinkler){
    if(controller == NULL || sprinkler == NULL){
        return;
    }
    if(controller->sprinkler_count >= MAX_LINKS_PER_CONTROLLER){
        return;
    }
    controller->sprinkler[controller->sprinkler_count] = sprinkler;
    controller->sprinkler_count++;
}

void SmokeController_Update(SmokeController *controller, uint64_t current_time){
    if(controller == NULL || controller->smoke_detector == NULL){
        return;
    }
    bool has_event = SmokeDetector_HasEvent(controller->smoke_detector);
    SmokeState smoke_state = SMOKE_NOT_DETECTED;

    if(has_event){
        smoke_state = SmokeDetector_Read(controller->smoke_detector);
        SmokeDetector_ClearEvent(controller->smoke_detector);
    }

    switch(controller->state){
        case SMOKE_CONTROL_ON:
            if(has_event && smoke_state == SMOKE_NOT_DETECTED){
                controller->state = SMOKE_DELAYED_OFF;
                controller->waiting_start_time = current_time;
            }
            break;

        case SMOKE_CONTROL_OFF:
            if(has_event && smoke_state == SMOKE_DETECTED){
                controller->waiting_start_time = current_time;
                controller->state = SMOKE_CONFIRMING;
            }
            break;
        
        case SMOKE_CONFIRMING:
            if(has_event && smoke_state == SMOKE_NOT_DETECTED){
                controller->state = SMOKE_CONTROL_OFF;
            }
            else if(current_time - controller->waiting_start_time >= SMOKE_CONFIRMATION_TIMEOUT_MS){
                controller->state = SMOKE_CONTROL_ON;
                TurnSprinklersOn(controller);
            }

            break;
        
        case SMOKE_DELAYED_OFF:
            if(has_event && smoke_state == SMOKE_DETECTED){
                controller->state = SMOKE_CONTROL_ON;
            }
            else if(current_time - controller->waiting_start_time >= SMOKE_DELAYED_OFF_TIMEOUT_MS){
                controller->state = SMOKE_CONTROL_OFF;
                TurnSprinklersOff(controller);
            }

            break;
        
        default:
            break;
    }
}

SmokeControlState SmokeController_GetState(const SmokeController *controller){
    if(controller == NULL){
        return SMOKE_CONTROL_OFF;
    }

    return controller->state;
}

static void TurnSprinklersOn(SmokeController *controller){
    for(int i = 0; i < controller->sprinkler_count; i++){
        Sprinkler_TurnOn(controller->sprinkler[i]);
    }
}

static void TurnSprinklersOff(SmokeController *controller){
    for(int i = 0; i < controller->sprinkler_count; i++){
        Sprinkler_TurnOff(controller->sprinkler[i]);
    }
}